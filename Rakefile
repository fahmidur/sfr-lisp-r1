require 'fileutils'
require 'pry'

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

def mimetype(fpath)
  name, mimetype = `file --mime-type #{fpath}`.split(/\s*\:\s*/)
  return mimetype
end

def deps(fnames)
  fnames = fnames.flatten.uniq
  #fnames.each do |name|
    #if File.exist?(name)
      #fpath = File.absolute_path(name)
      #fpath_mimetype = mimetype(fpath)
      #puts "  fpath=#{fpath}"
      #puts "  mimetype=#{fpath_mimetype}"
    #end
  #end
  return fnames
end

class Deps

  KNOWN_EXTS = ['.h', '.c']

  def initialize(names)
    @names = []
    if names.is_a?(String) || names.is_a?(Symbol)
      names = [names.to_s]
    end
    
    names.each do |e|
      if File.exist?(e)
        @names << e
      else
        basename = File.basename(e)
        candidates = KNOWN_EXTS.map {|ext| e+ext}
        hit = false
        candidates.each do |c|
          next unless File.exist?(c)
          @names << c 
          hit = true
        end
        unless hit
          raise "Deps. Could not resolve '#{e}'"
        end
      end
    end
    @names.uniq!
  end

  def ext(x)
    ext = ".#{x}".gsub(/\.+/, '.')
    Deps.new(@names.map do |e|
      xfile = File.basename(e, File.extname(e)) + ext
      if File.exist?(xfile)
        xfile
      else
        nil
      end
    end.compact.uniq)
  end

  def method_missing(meth, *args, &block)
    ext(meth)
  end

  def to_a
    @names
  end

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

def compile_file_task(type, target, deplist)
  basename = File.basename(target)
  desc "Build #{type} #{basename}"
  deplist = deps(deplist)
  file target => deplist do
    compile(type, target, deplist)
  end
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

util_names = ['Util']
util_names.each do |name|
  compile_file_task(:object, build(name+'.o'), [name+'.h', name+'.c'])
end

basic_types = ['Symbol', 'Number', 'String', 'Error', 'List', 'Hash']
basic_types.each do |name|
  compile_file_task(:object, build(name+'.o'), [name+'.c', name+'.h', 'Util.h'])
end

desc "Build Object object"
obj_basenames = ['Symbol', 'String', 'Number', 'Error', 'List', 'Hash'];
obj_hfiles = obj_basenames.map {|e| "#{e}.h" }
obj_ofiles = obj_basenames.map {|e| build("#{e}.o") }
file build('Object.o') => ['Object.h', 'Object.c', *obj_hfiles] do
  sh "#{cc} #{cflags} -c -o #{build('Object.o')} Object.c"
end

binding.pry

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
leaky_test_deps = deps([*test_files, 'leaky.c'])
file build('leaky') => leaky_test_deps do
  compile(:program, build('leaky'), leaky_test_deps)
end

desc "Build Symbol_test program"
symbol_test_deps = deps([*test_files, build('Util.o'), 'Symbol_test.c', build('Symbol.o')])
file build('Symbol_test') =>  symbol_test_deps do
  compile(:program, build('Symbol_test'), symbol_test_deps)
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

atom_ofiles = ['Symbol', 'String', 'Number', 'Error'].map {|e| build("#{e}.o") }

desc "Build List_test program"
list_test_deps = deps([runtime_ofiles, 'List_test.c'])
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

# These are artificial Rake targets to build and run the test programs.  These
# hardly get used and add noise to our list of real Rake targets.
#$build_targets.each do |k, path|
  #next if File.extname(path) == ".o"
  #desc "Build then Run #{path}"
  #task "run/#{path}" => [path] do
    #unless File.exist?(path)
      #puts "ERROR: Expecting file: #{path}"
      #exit 1
    #end
    #sh "#{path}"
  #end
#end
