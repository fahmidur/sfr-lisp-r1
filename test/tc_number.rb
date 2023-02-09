require 'test/unit'
require_relative './util.rb'

class TcNumber < Test::Unit::TestCase
  def test_string
    out = Util.run("./build/Number_test")
    assert_equal(0, out.exitstatus)
  end
  def test_valgrind_string
    out = Util.valgrind("./build/Number_test")
    assert_equal(0, out.exitstatus)
  end
end
