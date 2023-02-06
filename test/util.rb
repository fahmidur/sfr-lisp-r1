module Util

  VALGRIND_EXITCODE = 9

  def self.run(cmd)
    puts "Util.run. cmd=#{cmd}"
    out = `#{cmd}`
    ecode = $?.exitstatus
    puts "Util.run. ecode=#{ecode}"
    return ecode
  end

  def self.valgrind(cmd)
    ecode = Util.run("valgrind --leak-check=full --error-exitcode=#{VALGRIND_EXITCODE} #{cmd}")
    return ecode
  end
end
