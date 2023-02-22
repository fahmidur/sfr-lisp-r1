require 'test/unit'
require_relative './util.rb'

class TcObject < Test::Unit::TestCase
  def test_object
    out = Util.run("./build/Object_test")
    assert_equal(0, out.exitstatus)
  end
  def test_valgrind_object
    out = Util.valgrind("./build/Object_test")
    assert_equal(0, out.exitstatus)
  end
end
