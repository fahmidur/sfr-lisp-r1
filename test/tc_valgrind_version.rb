require 'test/unit'
require_relative './util.rb'

class TcValgrindVersion < Test::Unit::TestCase
  def test_valgrind_version
    out = Util.run("valgrind --version").stdout
    assert(out.size > 0)
    major, minor, patch = out[/[0-9\.]+/].split('.').map {|e| e.to_i }
    puts "major=#{major} minor=#{minor} patch=#{patch}"
    # latest version as of 2023-02-07
    assert(major >= 3)
    assert(minor >= 20)
  end
end
