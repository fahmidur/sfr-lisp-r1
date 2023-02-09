module Util

  VALGRIND_EXITCODE = 9

  class Shellout
    attr_reader :cmd
    attr_reader :exitstatus
    attr_reader :stdout

    def initialize(cmd)
      @cmd = cmd
      @exitstatus = nil
      @stdout = nil
    end
    def run!
      puts "Shellout. run! cmd: #{@cmd}"
      @stdout = `#{@cmd}`
      puts "--- { stdout { ---"
      puts @stdout
      puts "--- } stdout } ---"
      @exitstatus = $?.exitstatus
      return self
    end
  end

  def self.run(cmd)
    return Shellout.new(cmd).run!
  end

  def self.valgrind(cmd)
    return self.run("valgrind --leak-check=full --error-exitcode=#{VALGRIND_EXITCODE} #{cmd}")
  end
end
