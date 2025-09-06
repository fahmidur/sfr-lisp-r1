require 'yaml'

def empty?(x)
  x = x.to_str
  if x.size == 0 || x =~ /^\s*$/s 
    return true
  end
  return false
end

def read_version
  ret = `git describe --tags HEAD 2>/dev/null`.strip
  if empty?(ret)
    ret = "v0.0.0"
  end
  return ret
end

$git_sha = `git rev-parse HEAD`.strip
$version = read_version()

RAKEFILE_CONFIG_PATH = "RakefileConfig.yaml"
if File.exist?(RAKEFILE_CONFIG_PATH)
  $conf = YAML::load_file(RAKEFILE_CONFIG_PATH)
else
  $conf = {}
end

$target_to_inputs = {}

$dry = false
def dry
  $dry = true
  yield
  $dry = false
end

def runc(command)
  command = command.gsub(/\s+/m, ' ')
  if $dry
    puts "--- { runc.dry { ---"
    puts command
    puts "--- } runc.dry } ---"
  else
    sh(command)
  end
end

def env_truthy?(name)
  name = name.to_s
  val = ENV[name] || ENV[name.downcase] || ENV[name.upcase]
  return true if val == '1' || val == 't'
  return true if val =~ /true/i
  return false
end

def mimetype(fpath)
  name, mimetype = `file --mime-type #{fpath}`.split(/\s*\:\s*/)
  return mimetype
end

def deps(fnames)
  fnames = fnames.flatten.uniq
  return fnames
end

$build_targets = {}
$deps_by_path = {}
def build(name)
  name_ext = File.extname(name)
  unless ['.o', '.wasm', ''].member?(name_ext)
    raise "Invalid build file name: #{name}"
  end
  if $build_targets.has_key?(name)
    return $build_targets[name]
  end
  odir = 'build'
  unless Dir.exist?(odir)
    FileUtils.mkdir_p(odir)
  end
  target = File.join(odir, name)
  $build_targets[name] = target
  return target
end

def which(x)
  out = `which #{x}`.strip
  return out if out && out.size > 0
end

def has_flag?(name)
  name = name.to_s
  return !!(
    env_truthy?(name) ||
    ARGV.map{|e| e.gsub(/^\-+/, '').downcase }.member?(name)
  )
end

$debug = has_flag?(:debug)
puts "debug=#{$debug}"

$compilers = [ENV['CC'], "clang", "gcc"].compact
$cc = $compilers.find {|e| which(e) }
unless $cc
  puts "ERROR: failed to find a compiler in list: #{$compilers}"
  exit 1
end

$cflags = ["-g", "-I."]
# $cflags << "-fsanitize=address"
if $debug
  $cflags << "-D DEBUG"
end
if has_flag?('asan')
  $cflags << "-fsanitize=address"
end
# $cflags = $cflags.join(' ')

def wasm_conf_check
  wasm_err = []
  if $conf['wasm_cc']
    unless File.exist?($conf['wasm_cc'])
      wasm_err << "wasm. wasm_cc. No such file at #{$conf['wasm_cc']}"
    end
  else
    wasm_err << 'wasm. Missing conf :wasm_cc'
  end
  wasm_sysroot = $conf['wasm_sysroot']
  if wasm_sysroot
    wasm_sysroot = File.expand_path(wasm_sysroot)
    unless Dir.exist?(wasm_sysroot)
      wasm_err << "wasm. Expecting dir wasm_sysroot at #{wasm_sysroot}"
    end
  else
    wasm_err << 'wasm. Missing conf :wasm_sysroot'
  end
  unless $conf['wasm_target']
    wasm_err << 'wasm. Missing conf :wasm_target'
  end
  return wasm_err
end

def compile(type, ofile, sources)
  cc = $cc
  use_wasm = false
  if type.to_s =~ /^wasm/
    use_wasm = true
    wasm_err = wasm_conf_check()
    unless File::extname(ofile) == '.wasm'
      wasm_err << 'WASM ofile must end in .wasm'
    end
    if wasm_err.size > 0 
      puts "=== WASM Related Errors: ===\n"
      wasm_err.each do |err|
        puts "* #{err}"
      end
      exit 1
    end
    cc = $conf['wasm_cc']
  end

  cflags = $cflags.clone
  if use_wasm
    # address sanitizer is not supported for wasm targets
    cflags = cflags.reject {|e| e == "-fsanitize=address" }
  end

  com = <<~EOS
    #{cc} 
    #{cflags.join(' ')}
    -DVERSION='"#{$version}"'
    -DGIT_SHA='"#{$git_sha}"'
  EOS
  if type == :program 
    com += ""
  elsif type == :object 
    com += " -c "
  elsif type == :wasm_program
    com += " -D WASM"
    com += " -O3"
    com += " -target #{$conf['wasm_target']}"
    com += " --sysroot=#{File.expand_path($conf['wasm_sysroot'])}"
    com += " -nodefaultlibs"
    com += " -lc"
    com += " -lclang_rt.builtins-wasm32"
  else
    raise "compile(). Invalid type=#{type}"
  end
  com += " -o #{ofile} "

  sources = sources.flatten.uniq
  # Remove header files.
  # This allows us to use the same dependency list
  # as used by the calling Task.
  sources.select! {|e| File.extname(e) != '.h'}
  com += sources.join(' ')
  runc(com)
end

def inputs_for(targets)
  out = []
  unless targets.is_a?(Array)
    targets = [targets]
  end
  targets.each do |e|
    out << $target_to_inputs[e]
  end
  return out.flatten.compact.uniq
end

def compile_file_task(type, target, deplist)
  basename = File.basename(target)
  typename = sprintf("%-9s", type);
  desc "Build #{typename} : #{basename}"
  deplist = deps(deplist)
  # ensure that target is not itself in the deplist
  deplist = deplist - [target]
  $target_to_inputs[target] = deplist
  dry_save = $dry
  file(target => deplist) do
    $dry = dry_save
    compile(type, target, deplist)
  end
end

desc "Build everything"
task :build do
  $build_targets.each do |name, target|
    Rake::Task[target].invoke
  end
end

desc "Print build information"
task "print" do
  puts "compilers=#{$compilers}"
  puts "cc=#{$cc}"
end


