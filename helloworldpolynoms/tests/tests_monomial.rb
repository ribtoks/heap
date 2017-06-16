def require_path(pathes)
  require File.join(File.expand_path(File.dirname(__FILE__)), File.join(pathes))
end

require 'test/unit'
require_path ['..', 'parser.rb']

class MonomialTests < Test::Unit::TestCase
  def test_monomial_parse_power3x
    m = PolynomialParser.parse_monomial "3*x"
    assert_equal 1, m.powers['x']
    assert_equal 3, m.coef
  end

  def test_monomial_number
    m = PolynomialParser.parse_monomial "2"
    assert_equal 2, m.coef
    assert_equal 0, m.powers.size
  end

  def test_monomial_negative_number
    m = PolynomialParser.parse_monomial "-5"
    assert_equal -5, m.coef
    assert_equal 0, m.powers.size
  end

  def test_monomial_one_var
    m = PolynomialParser.parse_monomial "x"
    assert_equal 1, m.coef
    assert_equal 1, m.powers['x']
  end

  def test_monomial_negative_one_var
    m = PolynomialParser.parse_monomial "-x"
    assert_equal -1, m.coef
    assert_equal 1, m.powers['x']
  end

  def test_parse_empty
    m = PolynomialParser.parse_monomial ""
    assert m.powers.empty?, "Not empty"
  end

  def test_monomial_negative_simple
    m = PolynomialParser.parse_monomial "-3*x"
    assert_equal -3, m.coef
    assert_equal 1, m.powers['x']
  end

  def test_monomial_negative_simple_tricky
    m = PolynomialParser.parse_monomial "-x*3"
    assert_equal -3, m.coef
    assert_equal 1, m.powers['x']
  end

  def test_monomial_negative_quitesimple
    m = PolynomialParser.parse_monomial "-3*x*x"
    assert_equal -3, m.coef
    assert_equal 2, m.powers['x']
  end

  def test_monomial_power_simple
    m = PolynomialParser.parse_monomial "x^2"
    assert_equal 1, m.coef
    assert_equal 2, m.powers['x']
  end

  def test_monomial_power_quitesimple
    m = PolynomialParser.parse_monomial "x^2*y^3"
    assert_equal 1, m.coef
    assert_equal 2, m.powers['x']
    assert_equal 3, m.powers['y']
  end

  def test_monomial_power_raise_in_end
    assert_raise ParserException do
      m = PolynomialParser.parse_monomial "x^2*y^"
    end
  end

  def test_monomial_power_raise_in_middle
    assert_raise ParserException do
      m = PolynomialParser.parse_monomial "x^2*y^x"
    end
  end

  def test_monomial_simple_power
    m = PolynomialParser.parse_monomial "-3*x^2"
    assert_equal -3, m.coef
    assert_equal 2, m.powers['x']
  end

  def test_monomial_zero_power
    m = PolynomialParser.parse_monomial "x^0"
    assert_equal 1, m.coef
    assert_equal 0, m.powers['x']
  end

  def test_monomial_empty_power_simple
    assert_raise ParserException do
      m = PolynomialParser.parse_monomial "x^"
    end
  end

  def test_monomial_simple_multiple_power
    m = PolynomialParser.parse_monomial "-3*x^2*x*x*y*x^4"
    assert_equal -3, m.coef
    assert_equal 8, m.powers['x']
    assert_equal 1, m.powers['y']
  end

  def test_monomial_negative_quitesimple
    m = PolynomialParser.parse_monomial "-3*x*x"
    assert_equal -3, m.coef
    assert_equal 2, m.powers['x']
  end

  def test_monomial_complex_negative_various_vars
    m = PolynomialParser.parse_monomial "-30*x*x*y*x*z*6"
    assert_equal -180, m.coef
    assert_equal 3, m.powers['x']
    assert_equal 1, m.powers['y']
    assert_equal 1, m.powers['z']
  end

  def test_monomial_power_various_vars
    m = PolynomialParser.parse_monomial "-30*x^3*y*z*6"
    assert_equal -180, m.coef
    assert_equal 3, m.powers['x']
    assert_equal 1, m.powers['y']
    assert_equal 1, m.powers['z']
  end

  def test_monomial_complex_negative_various_vars_whitespace
    m = PolynomialParser.parse_monomial "  - 3  *  x*  x*y*x  *z*z*  7"
    assert_equal -21, m.coef
    assert_equal 3, m.powers['x']
    assert_equal 1, m.powers['y']
    assert_equal 2, m.powers['z']
  end

  def test_monomial_complex_various_vars
    m = PolynomialParser.parse_monomial "3*1*x*1*y*x*z"
    assert_equal 3, m.coef
    assert_equal 2, m.powers['x']
    assert_equal 1, m.powers['y']
    assert_equal 1, m.powers['z']
  end
  
  def test_monomial_complex_various_vars_whitespace
    m = PolynomialParser.parse_monomial "   3  *  1*  x*y*x  *z*z*  7"
    assert_equal 21, m.coef
    assert_equal 2, m.powers['x']
    assert_equal 1, m.powers['y']
    assert_equal 2, m.powers['z']
  end

  def test_monomial_parse_error_unknown_symbol
    assert_raise ParserException do
      m = PolynomialParser.parse_monomial "   3  *.  1*  x*y*x  *z*z*  7"
    end
  end

  def test_monomial_parse_error_no_terminal_symbol
    assert_raise ParserException do
      m = PolynomialParser.parse_monomial "   3    1*  x*y*x  *z*z*  7"
    end    
  end

  def test_monomial_to_string_simple
    m = Monomial.new({'x'=>1}, coef=1)
    assert_equal "x", m.to_s
  end

  def test_monomial_to_string_simple_negative
    m = Monomial.new({'x'=>1}, coef=-1)
    assert_equal "-x", m.to_s
  end

  def test_monomial_to_string_coef
    m = Monomial.new({'x'=>0}, coef=3)
    assert_equal "3", m.to_s
  end

  def test_monomial_to_string_coef_negative
    m = Monomial.new({'x'=>0}, coef=-3)
    assert_equal "-3", m.to_s
  end

  def test_monomial_to_string_coef_one
    m = Monomial.new({'x'=>0}, coef=1)
    assert_equal "1", m.to_s
  end
  
  def test_monomial_to_string_coef_one_negative
    m = Monomial.new({'x'=>0}, coef=-1)
    assert_equal "-1", m.to_s
  end

  def test_monomial_to_string_simple_power_negative
    m = Monomial.new({'x'=>2}, coef=-3)
    assert_equal "-3*x^2", m.to_s
  end
  
  def test_monomial_to_string_simple_power_positive
    m = Monomial.new({'x'=>8}, coef=6)
    assert_equal "6*x^8", m.to_s
  end

  def test_monomial_to_string_complex_no_coef
    m = Monomial.new({'x'=>2, 'y'=>3}, coef=1)
    assert_equal "x^2*y^3", m.to_s
  end

  def test_monomial_to_string_complex_with_coef
    m = Monomial.new({'x'=>2, 'y'=>3}, coef=4)
    assert_equal "4*x^2*y^3", m.to_s
  end

  def test_monomial_to_string_complex_with_negative_coef
    m = Monomial.new({'x'=>2, 'y'=>3}, coef=-1)
    assert_equal "-x^2*y^3", m.to_s
  end

  def test_monomial_to_string_complex_with_filter
    m = Monomial.new({'x'=>2, 'y'=>0, 'z'=>4}, coef=-2)
    assert_equal "-2*x^2*z^4", m.to_s
  end

  def test_monomial_to_string_complex_multivar
    m = Monomial.new({'x'=>2, 'y'=>0, 'u'=>1, 'z'=>4, 'k'=>3, 's'=>1}, coef=-2)
    assert_equal "-2*x^2*u*z^4*k^3*s", m.to_s
  end

  def test_monomial_to_string_zero_coef
    m = Monomial.new({'x'=>2, 'y'=>0, 'u'=>1, 'z'=>4, 'k'=>3, 's'=>1}, coef=0)
    assert_equal "0", m.to_s
  end

  def test_monomial_to_string_default
    assert_equal "0", Monomial.new.to_s
  end

  def test_monomial_constructor_default
    m = Monomial.new
    assert m.powers.empty?, "Powers are not empty"
    assert m.coef.zero?, "Coef is not zero"
  end

  def test_monomial_powers_same
    m1 = Monomial.new({'x'=>3, 'y'=>2}, 1)
    m2 = Monomial.new({'y'=>2, 'x'=>3}, 2)

    assert m1.equal_power?(m2), "Powers are not same"
  end

  def test_monomial_size_same
    m1 = Monomial.new({'x'=>3, 'y'=>2}, 1)
    m2 = Monomial.new({'y'=>2, 'x'=>3}, 2)

    assert m1.size == m2.size, "Sizes are not same"
  end

  def test_monomial_size_not_same
    m1 = Monomial.new({'x'=>3, 'y'=>2, 'z'=>1}, 1)
    m2 = Monomial.new({'y'=>2, 'x'=>3}, 2)

    assert m1.size != m2.size, "Sizes are same"
  end

  def test_monomial_equal_complex
    m1 = Monomial.new({'x'=>3, 'y'=>2}, -3)
    m2 = Monomial.new({'y'=>2, 'x'=>3}, -3)

    assert m1 == m2, "Monomials are not same"
  end

  def test_monomial_equal_simple
    m1 = Monomial.new({}, -3)
    m2 = Monomial.new({}, -3)

    assert m1 == m2, "Monomials are not same"
  end

  def test_multiplication_coef_simple
    m = Monomial.new({}, -3)
    assert_equal "-9", (m*3).to_s
  end

  def test_multiplication_coef_complex
    m = Monomial.new({'x'=>1, 'y'=>5}, -3)
    assert_equal "-9*x*y^5", (m*3).to_s
  end

  def test_multiplication_monomial_simple
    m1 = Monomial.new({'x'=>1}, -3)
    m2 = Monomial.new({'x'=>1}, -3)
    assert_equal "9*x^2", (m1*m2).to_s
  end

  def test_multiplication_monomial_complex
    m1 = Monomial.new({'x'=>1, 'y'=>2}, -3)
    m2 = Monomial.new({'x'=>3, 'z'=>1}, -3)
    assert_equal "9*x^4*y^2*z", (m1*m2).to_s
  end

  def test_multiplication_monomial_complex_zero_result
    m1 = Monomial.new({'x'=>1, 'y'=>2}, -3)
    m2 = Monomial.new({'x'=>3, 'z'=>1}, 0)
    assert_equal "0", (m1*m2).to_s
  end

  def test_multiplication_monomial_raises
    assert_raise NotImplementedError do
      m = Monomial.new({}, 3)
      m*"3"
    end
  end
end
