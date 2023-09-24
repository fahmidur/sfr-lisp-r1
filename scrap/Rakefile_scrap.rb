
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
