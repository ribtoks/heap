def require_path(pathes)
  require File.join(File.expand_path(File.dirname(__FILE__)), File.join(pathes))
end


require 'test/unit'
require_path ['..', 'parser.rb']

class PolynomialTests < Test::Unit::TestCase
  def test_parse_single
    p = PolynomialParser.parse_polynomial "3*x"
    assert p.monomials.size == 1, "Not one monomial"
    assert_equal 1, p.monomials.first.powers['x']
    assert_equal 3, p.monomials.first.coef
  end

  def test_parse_single_simple
    p = PolynomialParser.parse_polynomial "2"
    assert p.monomials.size == 1, "Not one monomial"
    assert_equal 0, p.monomials.first.powers['x']
    assert_equal 2, p.monomials.first.coef
  end

  def test_parse_single_complex
    p = PolynomialParser.parse_polynomial "  -3 * 33* x*x*u*x    * 3 *d"
    assert p.monomials.size == 1, "More than one monomial"
    assert_equal 3, p.monomials.first.powers['x']
    assert_equal 1, p.monomials.first.powers['u']
    assert_equal 1, p.monomials.first.powers['d']
    assert_equal -297, p.monomials.first.coef
  end

  def test_parse_simple_sum
    p = PolynomialParser.parse_polynomial "3*x + 6*y"
    assert p.monomials.size == 2, "Not two monomials"
    assert_equal 1, p.monomials.first.powers['x']
    assert_equal 3, p.monomials.first.coef

    assert_equal 1, p.monomials[1].powers['y']
    assert_equal 6, p.monomials[1].coef
  end

  def test_parse_simple_minus
    p = PolynomialParser.parse_polynomial "3*x - 6*y"
    assert p.monomials.size == 2, "Not two monomials"
    assert_equal 1, p.monomials.first.powers['x']
    assert_equal 3, p.monomials.first.coef

    assert_equal 1, p.monomials[1].powers['y']
    assert_equal -6, p.monomials[1].coef
  end

  def test_parse_simple_negative_minus
    p = PolynomialParser.parse_polynomial "   -3 *  x*  x - 6   * y"
    assert p.monomials.size == 2, "Not two monomials"
    assert_equal 2, p.monomials.first.powers['x']
    assert_equal -3, p.monomials.first.coef

    assert_equal 1, p.monomials[1].powers['y']
    assert_equal -6, p.monomials[1].coef
  end

  def test_parse_complex_minus_coef
    p = PolynomialParser.parse_polynomial "   -3 *  x*  x - 6   * y -23  +  2"
    assert p.monomials.size == 3, "Not four monomials"
    assert_equal 2, p.monomials.first.powers['x']
    assert_equal -3, p.monomials.first.coef

    assert_equal 1, p.monomials[1].powers['y']
    assert_equal -6, p.monomials[1].coef

    assert_equal -21, p.monomials[2].coef
  end

  def test_parse_complex_no_whitespace
    p = PolynomialParser.parse_polynomial "3*x*x*y*z+8*u-4*x+l"
    assert p.monomials.size == 4, "Not four monomials"
    assert_equal 3, p.monomials.first.coef
    assert_equal 2, p.monomials.first.powers['x']
    assert_equal 1, p.monomials.first.powers['y']
    assert_equal 1, p.monomials.first.powers['z']

    assert_equal 1, p.monomials[1].powers['u']
    assert_equal 8, p.monomials[1].coef

    assert_equal 1, p.monomials[2].powers['x']
    assert_equal -4, p.monomials[2].coef

    assert_equal 1, p.monomials[3].powers['l']
    assert_equal 1, p.monomials[3].coef
  end

  def test_parse_complex_with_whitespace
    p = PolynomialParser.parse_polynomial "   3*x*x*y*z+   8*u   -   4*x +l   "
    assert p.monomials.size == 4, "Not four monomials"
    assert_equal 3, p.monomials.first.coef
    assert_equal 2, p.monomials.first.powers['x']
    assert_equal 1, p.monomials.first.powers['y']
    assert_equal 1, p.monomials.first.powers['z']

    assert_equal 1, p.monomials[1].powers['u']
    assert_equal 8, p.monomials[1].coef

    assert_equal 1, p.monomials[2].powers['x']
    assert_equal -4, p.monomials[2].coef

    assert_equal 1, p.monomials[3].powers['l']
    assert_equal 1, p.monomials[3].coef
  end

  def test_parse_empty
    p = PolynomialParser.parse_polynomial ""
    assert p.monomials.empty?, "Monomials are not empty"
  end

  def test_compress_simple
    p = PolynomialParser.parse_polynomial "3*x + 6*x"

    assert p.monomials.size == 1, "Not one monomial"
    assert_equal 9, p.monomials.first.coef
    assert_equal 1, p.monomials.first.powers['x']
  end

  def test_compress_complex_one_last
    p = PolynomialParser.parse_polynomial "3*x*y*x - 4*f^3 + 6*x + 4 * f^3 +2*y*x*x - 6* x"
    
    assert_equal 1, p.monomials.size
    assert_equal 5, p.monomials.first.coef
    assert_equal 2, p.monomials.first.powers['x']
    assert_equal 1, p.monomials.first.powers['y']
  end

  def test_compress_complex_all_same
    p = PolynomialParser.parse_polynomial "3*x*y*x - 3*x^2*y +2*y*x*x - y*x*x"

    assert_equal 1, p.monomials.size
    assert_equal 1, p.monomials.first.coef
    assert_equal 2, p.monomials.first.powers['x']
    assert_equal 1, p.monomials.first.powers['y']
  end

  def test_compress_no_compress
    p = PolynomialParser.parse_polynomial "3*x*y*x*f - 3*x*x*y*j +2*y*x*x*k -l* y*x*x"

    assert_equal 4, p.monomials.size
  end

  def test_compress_numbers
    p = PolynomialParser.parse_polynomial "3+1 + 4 + 5*6 - 4 - 2*2"

    assert_equal 1, p.monomials.size
    assert_equal 30, p.monomials.first.coef
    assert p.monomials.first.size.zero?, "Has some powers"
  end

  def test_polynomials_equal_simple
    p1 = PolynomialParser.parse_polynomial "3*x*y*x - 4*f^3 + 6*x + 4 * f*f*f +2*y*x*x - 6* x"
    p2 = PolynomialParser.parse_polynomial "5*x*y*x"

    assert p1 == p2, "Polynomials not equal [#{p1.to_s}] vs [#{p2.to_s}]"
  end

  def test_polynomials_equal_numbers
    p1 = PolynomialParser.parse_polynomial "3 - 4+ 6+ 4 +2 - 6"
    p2 = PolynomialParser.parse_polynomial "5"

    assert p1 == p2, "Numbers not equal [#{p1.to_s}] vs [#{p2.to_s}]"
  end

  def test_polynomials_equal_complex
    p1 = PolynomialParser.parse_polynomial "-2*y*y*z*k - 3*4*i*z*z + 5 - 2*x + x*x - y + p"
    p2 = PolynomialParser.parse_polynomial "p + x*x - 12*z*i*z - 2*k*z*y*y + 2 - x + 3 - x - y"

    assert p1 == p2, "Polynomials not equal [#{p1.to_s}] vs [#{p2.to_s}]"
  end

  def test_add_monomials_simple
    p1 = PolynomialParser.parse_polynomial "2*x"
    p2 = PolynomialParser.parse_polynomial "3*y"
    expected = PolynomialParser.parse_polynomial "2*x+3*y"
    actual = p1 + p2
    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end

  def test_add_monomials_number
    p1 = PolynomialParser.parse_polynomial "2*x*y"
    expected = PolynomialParser.parse_polynomial "2*y*x + 5"
    actual = p1 + 5
    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end

  def test_add_monomials_complex
    p1 = PolynomialParser.parse_polynomial "-2*x*i*o*x + 4*x*l*l"
    p2 = PolynomialParser.parse_polynomial "x*i*x*o - 2*x*l*l*2"
    expected = PolynomialParser.parse_polynomial "-x*o*i*x"
    actual = p1 + p2
    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end

  def test_mul_polynomial_number
    p1 = PolynomialParser.parse_polynomial "x*i*o*x"
    expected = PolynomialParser.parse_polynomial "-x*o*i*x*2"
    actual = p1 * (-2)
    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end

  def test_mul_complex_polynomial_number
    p1 = PolynomialParser.parse_polynomial "x*i*o*x + 7*x*x"
    expected = PolynomialParser.parse_polynomial "-x*o*i*x*2 -14*x*x"
    actual = p1 * (-2)
    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end

  def test_mul_polynomial_polynomial_simple
    p1 = PolynomialParser.parse_polynomial "x+y"
    p2 = PolynomialParser.parse_polynomial "x-y"
    expected = PolynomialParser.parse_polynomial "x*x-y*y"
    actual = p1 * p2
    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end

  def test_mul_polynomial_polynomial_simple_cube_plus
    p1 = PolynomialParser.parse_polynomial "x+y"
    p2 = PolynomialParser.parse_polynomial "x*x-x*y+y*y"
    expected = PolynomialParser.parse_polynomial "x^3+y^3"
    actual = p1 * p2
    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end

  def test_mul_polynomial_polynomial_simple_cube_minus
    p1 = PolynomialParser.parse_polynomial "x-y"
    p2 = PolynomialParser.parse_polynomial "x*x+x*y+y*y"
    expected = PolynomialParser.parse_polynomial "x^3-y^3"
    actual = p1 * p2
    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end

  def test_mul_polynomial_monomial
    p = PolynomialParser.parse_polynomial "x-y"
    m = PolynomialParser.parse_monomial "-4*x*u*x"
    expected = PolynomialParser.parse_polynomial "4*x^2*y*u-4*x^3*u"
    actual = p * m
    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end

  def test_mul_polynomial_polynomial_complex
    p1 = PolynomialParser.parse_polynomial "-5*x*x*y-10*x*y+y*y*y"
    p2 = PolynomialParser.parse_polynomial "4*x*y*y-2*x*y*y*y"
    expected = PolynomialParser.parse_polynomial "-20*x^3*y^3 +10*x^3*y^4 - 40*x^2*y^3+20*x^2*y^4+4*x*y^5-2*x*y^6"
    actual = p1 * p2

    assert expected == actual, "Polynomials are not equal [#{expected.to_s}] vs [#{actual.to_s}]"
  end
end
