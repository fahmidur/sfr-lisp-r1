
def env_truthy?(name)
  name = name.to_s
  val = ENV[name] || ENV[name.downcase]
  return true if val == '1' || val == 't'
  return true if val =~ /true/i
  return false
end

def runc(command)
  command = command.gsub(/\s+/m, ' ')
  sh(command)
end

#def mimetype(fpath)
  #name, mimetype = `file --mime-type #{fpath}`.split(/\s*\:\s*/)
  #return mimetype
#end

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

def which(x)
  out = `which #{x}`.strip
  return out if out && out.size > 0
  return nil
end

