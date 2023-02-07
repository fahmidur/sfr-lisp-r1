require 'test/unit'
require_relative './util.rb'

class TcSymbol < Test::Unit::TestCase
  def test_symbols
    out = Util.run("./build/Symbol_test")
    assert_equal(0, out.exitstatus)
  end
  def test_valgrind_symbols
    out = Util.valgrind("./build/Symbol_test")
    assert_equal(0, out.exitstatus)
  end
end
