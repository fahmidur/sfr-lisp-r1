require 'set'
def env_truthy?(name)
  name = name.to_s
  val = ENV[name] || ENV[name.downcase]
  return true if val == '1' || val == 't'
  return true if val =~ /true/i
  return false
end

$build_targets = {}
def build(name)
  if $build_targets.has_key?(name)
    return $build_targets[name]
  end
  target = "build/#{name}"
  $build_targets[name] = target
  return target
end

debug = env_truthy?(:debug) ? true : false
cc = ENV['CC'] || "clang"
cflags = ["-g", "-I."]
if debug
  cflags << "-D DEBUG"
end
cflags = cflags.join(' ')

task :default => :build

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

desc "Build nassert_test program"
file build('nassert_test') => ['nassert.h', 'nassert_test.c'] do
  sh "#{cc} #{cflags} -o #{build('nassert_test')} nassert_test.c"
end

desc "Build leaky test program."
file build('leaky') => ['leaky.c'] do
  sh "#{cc} #{cflags} -o #{build('leaky')} leaky.c"
end

desc "Build Util object"
file build("Util.o") => ["Util.h", "Util.c"] do
  sh "#{cc} #{cflags} -c -o #{build('Util.o')} Util.c"
end

desc "Build Symbol object"
file build('Symbol.o') =>  ['Symbol.c', 'Symbol.h', 'Util.h'] do
  sh "#{cc} #{cflags} -c -o #{build('Symbol.o')} Symbol.c"
end

desc "Build Symbol_test program"
file build('Symbol_test') => ['Symbol_test.c', build('Symbol.o')] do
  sh "#{cc} #{cflags} -o #{build('Symbol_test')} Symbol_test.c #{build('Symbol.o')} #{build('Util.o')}"
end

desc "Build Number object"
file build('Number.o') => ['Number.c', 'Number.h'] do
  sh "#{cc} #{cflags} -c -o #{build('Number.o')} Number.c"
end

desc "Build Number_test program"
file build("Number_test") => ['Number_test.c', build('Number.o')] do
  sh "#{cc} #{cflags} -o #{build('Number_test')} Number_test.c #{build('Number.o')}"
end

desc "Build String object"
file build('String.o') => ['String.c', 'String.h', 'Util.h'] do
  sh "#{cc} #{cflags} -c -o #{build('String.o')} String.c"
end

desc "Build String_test program"
file build('String_test') => ['String_test.c', build('String.o')] do
  sh "#{cc} #{cflags} -o #{build('String_test')} String_test.c #{build('String.o')} #{build('Util.o')}"
end

desc "Build List object"
file build('List.o') => ['List.h', 'List.c'] do
  sh "#{cc} #{cflags} -c -o #{build('List.o')} List.c"
end

desc "Build Object object"
obj_basenames = ['Symbol', 'String', 'Number', 'List'];
obj_hfiles = obj_basenames.map {|e| "#{e}.h" }
obj_ofiles = obj_basenames.map {|e| build("#{e}.o") }
file build('Object.o') => ['Object.h', 'Object.c', *obj_hfiles] do
  sh "#{cc} #{cflags} -c -o #{build('Object.o')} Object.c"
end

desc "Build Object_test program" 
file build('Object_test') => ['Object_test.c', build('Object.o')] do
  sh "#{cc} #{cflags} -o #{build('Object_test')} Object_test.c #{build('Object.o')} #{obj_ofiles.join(' ')} #{build('Util.o')}"
end

desc "Build List_test program"
atom_types = ['Symbol', 'String', 'Number'].map {|e| build("#{e}.o") }
file build('List_test') => ['List_test.c', build('List.o'), *atom_types, build('Object.o'), build('Util.o')] do
  sh "#{cc} #{cflags} -o #{build('List_test')} List_test.c #{build('List.o')} #{atom_types.join(' ')} #{build('Util.o')} #{build('Object.o')}"
end

desc "Build Tokenizer object"
file build('Tokenizer.o') => ['Tokenizer.c', 'Tokenizer.h'] do
  sh "#{cc} #{cflags} -c -o #{build('Tokenizer.o')} Tokenizer.c"
end

desc "Build Tokenizer_test program"
file build('Tokenizer_test') => ['Tokenizer_test.c', build('Tokenizer.o')] do
  sh "#{cc} #{cflags} -o #{build('Tokenizer_test')} Tokenizer_test.c #{build('Tokenizer.o')}"
end

desc "Build Util_test program"
file build('Util_test') => ['Util_test.c', build('Util.o')] do
  sh "#{cc} #{cflags} -o #{build('Util_test')} Util_test.c #{build('Util.o')}"
end

desc "Run all tests"
task :test => :build do
  sh "ruby ./test/all.rb"
end
