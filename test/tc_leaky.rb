require 'test/unit'
require_relative './util.rb'

class TcLeaky < Test::Unit::TestCase
  def test_leaky
    ecode = Util.valgrind("./build/leaky")
    assert_equal(Util::VALGRIND_EXITCODE, ecode)
  end
end
