require 'test/unit'
require_relative './util.rb'

class TcList < Test::Unit::TestCase
  def test_list
    out = Util.run("./build/List_test")
    assert_equal(0, out.exitstatus)
  end
  def test_valgrind_list
    out = Util.valgrind("./build/List_test")
    assert_equal(0, out.exitstatus)
  end
end