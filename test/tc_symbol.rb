require 'test/unit'
require_relative './util.rb'

class TcSymbol < Test::Unit::TestCase
  def test_symbols
    ecode = Util.run("./build/Symbol_test")
    assert_equal(0, ecode)
  end
  def test_valgrind_symbols
    ecode = Util.valgrind("./build/Symbol_test")
    assert_equal(0, ecode)
  end
end
