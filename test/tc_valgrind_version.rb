require 'test/unit'
require_relative './util.rb'

class TcValgrindVersion < Test::Unit::TestCase
  def test_valgrind_version
    out = Util.run("valgrind --version").stdout
    assert(out.size > 0)
    major, minor, patch = out[/[0-9\.]+/].split('.').map {|e| e.to_i }
    puts "major=#{major} minor=#{minor} patch=#{patch}"
    assert(major >= 3)
    #assert(minor >= 20)
    assert(minor >= 16)
  end
end
