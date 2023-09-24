require 'fileutils'
require 'pry'
require_relative './RakefileUtils.rb'

task :default => :build

desc "Print build information"
task "print" do
  puts "compilers=#{$compilers}"
  puts "cc=#{$cc}"
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

util_bnames = ['Util']
util_hfiles = util_bnames.map {|e| e+'.h' }
util_ofiles = util_bnames.map {|e| build(e+'.o') }
util_bnames.each do |name|
  compile_file_task(:object, build(name+'.o'), [name+'.h', name+'.c'])
end

basic_bnames = ['Symbol', 'String', 'Number', 'Error', 'List', 'Hash']
basic_ofiles = basic_bnames.map {|e| build(e+'.o') }
basic_hfiles = basic_bnames.map {|e| e+'.h' }
basic_bnames.each do |name|
  compile_file_task(:object, build(name+'.o'), [name+'.c', name+'.h', util_hfiles])
end

obj_hfiles = deps(['Object.h', basic_hfiles])
obj_ofiles = deps([build('Object.o'), basic_ofiles])
compile_file_task(:object, build('Object.o'), ['Object.c', *obj_hfiles])

runtime_ofiles = deps([build('Util.o'), obj_ofiles, build('Runtime.o')])
compile_file_task(:object, build('Runtime.o'), ['Runtime.c'])

compile_file_task(:object, build('Lisp.o'), ['Lisp.h', 'Lisp.c'])

compile_file_task(:object, build('Tokenizer.o'), ['Tokenizer.c', 'Tokenizer.h'])

#=========================================================
# Test Programs
#==========================================================

compile_file_task(:program, build('leaky'), ['leaky.c'])

test_hfiles = ['nassert.h']
test_common = [*test_hfiles, build('Util.o')]

compile_file_task(:program, build('nassert_test'), ['nassert.h', 'nassert_test.c'])

compile_file_task(:program, build('Util_test'), ['Util_test.c', build('Util.o'), test_common])

['Symbol', 'String', 'Number'].each do |name|
  compile_file_task(:program, build(name+'_test'), [name+'_test.c', build(name+'.o'), test_common])
end
['Object', 'List', 'Hash'].each do |name|
  compile_file_task(:program, build(name+'_test'), [name+'_test.c', obj_ofiles, test_common])
end

compile_file_task(:program, build('Runtime_test'), ['Runtime_test.c', runtime_ofiles, test_common])

compile_file_task(:program, build('Lisp_test'), ['Lisp_test.c', build('Lisp.o'), runtime_ofiles, test_common])

compile_file_task(:program, build('Tokenizer_test'), ['Tokenizer_test.c', build('Tokenizer.o'), test_common])
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
