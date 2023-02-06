require 'test/unit'
require_relative './util.rb'

class TcNassert < Test::Unit::TestCase
  def test_nassert
    ecode = Util.run("./build/nassert_test")
    assert_equal(0, ecode)
  end
  def test_valgrind_nassert
    ecode = Util.valgrind("./build/nassert_test")
    assert_equal(0, ecode)
  end
end
