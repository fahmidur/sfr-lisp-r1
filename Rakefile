require 'fileutils'

$git_sha = `git rev-parse HEAD`.strip
$version = `git describe --tags HEAD`.strip

def runc(command)
  command = command.gsub(/\s+/m, ' ')
  sh(command)
end

def env_truthy?(name)
  name = name.to_s
  val = ENV[name] || ENV[name.downcase]
  return true if val == '1' || val == 't'
  return true if val =~ /true/i
  return false
end

def deps(x)
  return x.flatten.uniq
end

$build_targets = {}
$deps_by_path = {}
def build(name)
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
  return nil
end

debug = env_truthy?(:debug) ? true : false
compilers = [ENV['CC'], "clang", "gcc"].compact
cc = compilers.find {|e| which(e) }
unless cc
  puts "ERROR: failed to find a compiler in list: #{compilers}"
  exit 1
end

cflags = ["-g", "-I."]
if debug
  cflags << "-D DEBUG"
end
cflags = cflags.join(' ')

$cc = cc
$cflags = cflags
def compile(type, ofile, sources)
  com = "#{$cc} #{$cflags} "
  if type == :program 
    com += ""
  elsif type == :object 
    com += " -c "
  else
    raise "Compile. Invalid type=#{type}"
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

task :default => :build

desc "Print build information"
task "print" do
  puts "compilers=#{compilers}"
  puts "cc=#{cc}"
end

desc "Clean all build artifacts"
task :clean do
  puts "clean. cleaning..."
  sh "mkdir -p ./build"
  sh "rm -rf ./build/*"
  sh "rm -f *.o"

  # Remove any stray executable files which 
  # my be in non-build directory
  $build_targets.each do |name, target|
    if File.exist?(name) && File.executable?(name)
      puts "stray found: #{name}"
      sh "rm #{name}"
    end
  end
end

desc "Build everything"
task :build do
  $build_targets.each do |name, target|
    Rake::Task[target].invoke
  end
end

desc "Build Util object"
file build("Util.o") => ["Util.h", "Util.c"] do
  sh "#{cc} #{cflags} -c -o #{build('Util.o')} Util.c"
end

desc "Build Symbol object"
file build('Symbol.o') =>  ['Symbol.c', 'Symbol.h', 'Util.h'] do
  sh "#{cc} #{cflags} -c -o #{build('Symbol.o')} Symbol.c"
end


desc "Build Number object"
file build('Number.o') => ['Number.c', 'Number.h'] do
  sh "#{cc} #{cflags} -c -o #{build('Number.o')} Number.c"
end

desc "Build String object"
file build('String.o') => ['String.c', 'String.h', 'Util.h'] do
  sh "#{cc} #{cflags} -c -o #{build('String.o')} String.c"
end

desc "Build Error object"
file build('Error.o') => ['Error.h', 'Error.c', 'Util.h'] do
  sh "#{cc} #{cflags} -c -o #{build('Error.o')} Error.c"
end

desc "Build List object"
file build('List.o') => ['List.h', 'List.c'] do
  sh "#{cc} #{cflags} -c -o #{build('List.o')} List.c"
end

desc "build Hash object"
file build('Hash.o') => ['Hash.h', 'Hash.c'] do
  sh "#{cc} #{cflags} -c -o #{build('Hash.o')} Hash.c"
end

desc "Build Object object"
obj_basenames = ['Symbol', 'String', 'Number', 'Error', 'List', 'Hash'];
obj_hfiles = obj_basenames.map {|e| "#{e}.h" }
obj_ofiles = obj_basenames.map {|e| build("#{e}.o") }
file build('Object.o') => ['Object.h', 'Object.c', *obj_hfiles] do
  sh "#{cc} #{cflags} -c -o #{build('Object.o')} Object.c"
end

#desc "Build Function object"
#file build('Function.o') => ['Function.h', 'Function.c'] do
  #sh "#{cc} #{cflags} -c -o #{build('Function.o')} Function.c"
#end

desc "Build Runtime object"
file build('Runtime.o') => [build('Object.o')] do
  sh "#{cc} #{cflags} -c -o #{build('Runtime.o')} Runtime.c"
end

desc "Build Lisp.o object"
file build('Lisp.o') => ['Lisp.c', 'Lisp.h'] do
  sh "#{cc} #{cflags} -c -o #{build('Lisp.o')} Lisp.c"
end

desc "Build Tokenizer object"
file build('Tokenizer.o') => ['Tokenizer.c', 'Tokenizer.h'] do
  sh "#{cc} #{cflags} -c -o #{build('Tokenizer.o')} Tokenizer.c"
end

#=================
# Test Programs
#=================

test_files = ['nassert.h']

desc "Build nassert_test program"
nassert_test_deps = [*test_files, 'nassert_test.c']
file build('nassert_test') =>  nassert_test_deps do
  compile(:program, build('nassert_test'), nassert_test_deps)
end

desc "Build leaky test program."
leaky_deps = deps([*test_files, 'leaky.c'])
file build('leaky') => leaky_deps do
  compile(:program, build('leaky'), leaky_deps)
end

desc "Build Symbol_test program"
file build('Symbol_test') => [*test_files, build('Util.o'), 'Symbol_test.c', build('Symbol.o')] do
  sh "#{cc} #{cflags} -o #{build('Symbol_test')} Symbol_test.c #{build('Symbol.o')} #{build('Util.o')}"
end

desc "Build String_test program"
file build('String_test') => [*test_files, build('Util.o'), 'String_test.c', build('String.o')] do
  sh "#{cc} #{cflags} -o #{build('String_test')} String_test.c #{build('String.o')} #{build('Util.o')}"
end

desc "Build Number_test program"
file build("Number_test") => [*test_files, build('Util.o'), 'Number_test.c', build('Number.o')] do
  sh "#{cc} #{cflags} -o #{build('Number_test')} Number_test.c #{build('Number.o')}"
end

desc "Build Object_test program" 
file build('Object_test') => [*test_files, build('Util.o'), 'Object_test.c', build('Object.o'), *obj_ofiles] do
  sh "#{cc} #{cflags} -o #{build('Object_test')} Object_test.c #{build('Object.o')} #{obj_ofiles.join(' ')} #{build('Util.o')}"
end

desc "Build Runtime_test program"
file build('Runtime_test') => [*test_files, build('Util.o'), 'Runtime_test.c', build('Runtime.o'), build('Util.o'), build('Object.o'), *obj_ofiles] do
  sh "#{cc} #{cflags} -o #{build('Runtime_test')} Runtime_test.c #{build('Runtime.o')} #{build('Object.o')} #{obj_ofiles.join(' ')} #{build('Util.o')}"
end

runtime_ofiles = [
  build('Util.o'),
  build('Object.o'),
  obj_ofiles,
  build('Runtime.o'),
].flatten.uniq

desc "Build List_test program"
atom_ofiles = ['Symbol', 'String', 'Number', 'Error'].map {|e| build("#{e}.o") }
list_test_deps = deps([runtime_ofiles, 'List_test.c'])
#file build('List_test') => [*test_files, build('Util.o'), 'List_test.c', build('List.o'), *atom_ofiles, build('Object.o')] do
  #sh "#{cc} #{cflags} -o #{build('List_test')} List_test.c #{build('List.o')} #{atom_ofiles.join(' ')} #{build('Util.o')} #{build('Object.o')}"
#end
file build('List_test') => list_test_deps do
  compile(:program, build('List_test'), list_test_deps)
end

desc "Build Hash_test program"
hash_test_deps = deps([
  *test_files, 
  build('Util.o'), 
  'Hash_test.c', 
  build('Hash.o'), 
  *atom_ofiles, 
  build('Object.o'),
  build('List.o')
])
file build('Hash_test') => hash_test_deps do
  compile(:program, build('Hash_test'), hash_test_deps)
end

# SFR: Marked for deletion.
desc "Build Tokenizer_test program"
file build('Tokenizer_test') => [*test_files, build('Util.o'), 'Tokenizer_test.c', build('Tokenizer.o')] do
  sh "#{cc} #{cflags} -o #{build('Tokenizer_test')} Tokenizer_test.c #{build('Tokenizer.o')}"
end

desc "Build Util_test program"
util_test_deps = deps([*test_files, 'Util_test.c', build('Util.o')])
file build('Util_test') =>  util_test_deps do
  #sh "#{cc} #{cflags} -o #{build('Util_test')} Util_test.c #{build('Util.o')}"
  compile(:program, build('Util_test'), util_test_deps)
end

desc "Build Lisp_test program"
lisp_test_deps = deps([*test_files, 'Lisp_test.c', build('Lisp.o'), *runtime_ofiles])
file build("Lisp_test") =>  lisp_test_deps do
  compile(:program, build('Lisp_test'), lisp_test_deps)
end

desc "Build sfr-lisp program"
file build('sfr-lisp') => ['sfr-lisp.c', build('Util.o'), build('Object.o'), *obj_ofiles, build('Tokenizer.o')] do
  runc <<~EOF
    #{cc} #{cflags}  
    -o #{build('sfr-lisp')} sfr-lisp.c 
    -DVERSION='"#{$version}"'
    -DGIT_SHA='"#{$git_sha}"'
    #{build('Object.o')} 
    #{obj_ofiles.join(' ')} 
    #{build('Util.o')} 
    #{build('Tokenizer.o')} 
  EOF
end

desc "Run all tests"
task :test => :build do
  sh "ruby ./test/all.rb"
end

$build_targets.each do |k, path|
  next if File.extname(path) == ".o"
  desc "Build then Run #{path}"
  task "run/#{path}" => [path] do
    unless File.exist?(path)
      puts "ERROR: Expecting file: #{path}"
      exit 1
    end
    sh "#{path}"
  end
end
