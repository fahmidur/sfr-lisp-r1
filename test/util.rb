module Util

  require 'fileutils'
  require 'ostruct'
  require 'digest'

  VALGRIND_EXITCODE = 9
  COVERAGE_DATA_DIR = "coverage_data"

  class Shellout
    require 'open3'
    attr_reader :env
    attr_reader :cmd
    attr_reader :exitstatus
    attr_reader :stdout
    attr_reader :stderr

    def initialize(env, cmd)
      @env = env
      @cmd = cmd
      @exitstatus = nil
      @stdout = nil
      @stderr = nil
    end

    def run!
      puts "Shellout. run! cmd: #{@cmd}"
      @stdout, @stderr, status = Open3.capture3(@env, @cmd)
      @exitstatus = status.exitstatus
      if @stdout && @stdout.size > 0
        puts "--- { stdout { ---"
        puts @stdout
        puts "--- } stdout } ---"
      end
      if @stderr && @stderr.strip.size > 0 
        puts "--- { stderr { ---"
        puts @stderr
        puts "--- } stderr } ---"
      end
      # @stdout = `#{@cmd}`
      # @exitstatus = $?.exitstatus
      return self
    end
  end

  def self.env_truthy?(name)
    name = name.to_s
    val = ENV[name] || ENV[name.downcase] || ENV[name.upcase]
    return true if ['1', 't', 'T'].member?(val)
    return true if val =~ /true/i
    return false
  end

  def self.has_flag?(name)
    name = name.to_s.downcase
    return !!(
      self.env_truthy?(name) ||
      ARGV.map{|e| e.gsub(/^\-+/, '').downcase }.member?(name)
    )
  end

  def self.run(cmd)
    env = {}
    if has_flag?(:cover)
      unless Dir.exist?(COVERAGE_DATA_DIR)
        FileUtils.mkdir_p(COVERAGE_DATA_DIR)
      end
      basename = File.basename(cmd)
      if basename.end_with?('_test') && File.exist?(cmd)
        fname = basename  + ".profraw"
        llvm_profile_file = File.join(COVERAGE_DATA_DIR, fname)
        puts "cover | llvm_profile_file: #{llvm_profile_file}"
        env['LLVM_PROFILE_FILE'] = llvm_profile_file
      end
    end
    return Shellout.new(env, cmd).run!
  end

  def self.valgrind_mock(cmd)
    exitstatus = 0
    if cmd == './build/leaky'
      # we expect this leaky program to fail valgrind
      exitstatus = VALGRIND_EXITCODE
    end
    return OpenStruct.new({
      :exitstatus => exitstatus,
    })
  end

  def self.valgrind(cmd)
    if self.has_flag?(:no_valgrind)
      return valgrind_mock(cmd)
    end
    return self.run("valgrind --leak-check=full --error-exitcode=#{VALGRIND_EXITCODE} #{cmd}")
  end
end
