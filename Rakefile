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
  desc "-- Build #{type}: #{basename}"
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

basic_bnames = ['Symbol', 'String', 'Number', 'Error', 'List', 'Hash']
basic_ofiles = basic_bnames.map {|e| build(e+'.o') }
basic_hfiles = basic_bnames.map {|e| e+'.h' }
basic_bnames.each do |name|
  compile_file_task(:object, build(name+'.o'), [name+'.c', name+'.h', 'Util.h'])
end

obj_hfiles = deps(['Object.h', basic_hfiles])
obj_ofiles = deps([build('Object.o'), basic_ofiles])
compile_file_task(:object, build('Object.o'), ['Object.c', *obj_hfiles])

runtime_ofiles = deps([build('Util.o'), obj_ofiles, build('Runtime.o')])
compile_file_task(:object, build('Runtime.o'), ['Runtime.c'])

compile_file_task(:object, build('Lisp.o'), ['Lisp.c'])

compile_file_task(:object, build('Tokenizer.o'), ['Tokenizer.c', 'Tokenizer.h'])

#=========================================================
# Test Programs
#==========================================================

test_hfiles = ['nassert.h']
test_common = [*test_hfiles, build('Util.o')]

compile_file_task(:program, build('nassert_test'), ['nassert.h', 'nassert_test.c'])

compile_file_task(:program, build('leaky'), ['leaky.c'])

['Symbol', 'String', 'Number'].map do |name|
  compile_file_task(:program, build(name+'_test'), [name+'_test.c', build(name+'.o'), test_common])
end

compile_file_task(:program, build('Object_test'), ['Object_test.c', obj_ofiles, test_common])

compile_file_task(:program, build('Runtime_test'), ['Runtime_test.c', runtime_ofiles, test_common])

compile_file_task(:program, build('List_test'), ['List_test.c', obj_ofiles, test_common])

compile_file_task(:program, build('Hash_test'), ['Hash_test.c', obj_ofiles, test_common])

compile_file_task(:program, build('Tokenizer_test'), ['Tokenizer_test.c', build('Tokenizer.o'), test_common])

compile_file_task(:program, build('Util_test'), ['Util_test.c', build('Util.o'), test_common])

compile_file_task(:program, build('Lisp_test'), ['Lisp_test.c', build('Lisp.o'), runtime_ofiles, test_common])

compile_file_task(:program, build('sfr-lisp'), ['sfr-lisp.c', runtime_ofiles, build('Tokenizer.o'), test_common])

desc "Run all tests"
task :test => :build do
  sh "ruby ./test/all.rb"
end

#==============================================================================
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
