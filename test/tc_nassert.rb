require 'test/unit'
require_relative './util.rb'

class TcNassert < Test::Unit::TestCase
  def test_nassert
    out = Util.run("./build/nassert_test")
    assert_equal(0, out.exitstatus)
  end
  def test_valgrind_nassert
    out = Util.valgrind("./build/nassert_test")
    assert_equal(0, out.exitstatus)
  end
end
