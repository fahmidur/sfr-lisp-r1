require 'test/unit'
require_relative './util.rb'

class TcLisp < Test::Unit::TestCase
  def test_lisp
    out = Util.run("./build/Lisp_test")
    assert_equal(0, out.exitstatus)
  end
  def test_valgrind_lisp
    out = Util.valgrind("./build/Lisp_test")
    assert_equal(0, out.exitstatus)
  end
end
