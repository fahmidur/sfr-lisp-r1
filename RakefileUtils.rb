$git_sha = `git rev-parse HEAD`.strip
$version = `git describe --tags HEAD`.strip

def runc(command)
  command = command.gsub(/\s+/m, ' ')
  sh(command)
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
  unless ['.o', ''].member?(name_ext)
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
# $cflags = ["-g", "-I.", "-fsanitize=address"]
if $debug
  $cflags << "-D DEBUG"
end
if has_flag?('asan')
  $cflags << "-fsanitize=address"
end
$cflags = $cflags.join(' ')

def compile(type, ofile, sources)
  com = <<~EOS
    #{$cc} #{$cflags}
    -DVERSION='"#{$version}"'
    -DGIT_SHA='"#{$git_sha}"'
  EOS
  if type == :program 
    com += ""
  elsif type == :object 
    com += " -c "
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

def compile_file_task(type, target, deplist)
  basename = File.basename(target)
  typename = sprintf("%-9s", type);
  desc "Build #{typename} : #{basename}"
  deplist = deps(deplist)
  # ensure that target is not itself in the deplist
  deplist = deplist - [target]
  file(target => deplist) do
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


