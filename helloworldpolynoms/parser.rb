def require_path(pathes)
  require File.join(File.expand_path(File.dirname(__FILE__)), File.join(pathes))
end

require_path ['monomial.rb']
require_path ['polynomial.rb']
require_path ['parser_helpers.rb']

class ParserException < Exception
  def initialize(text)
    @message = text
  end
end

class PolynomialParser
  class << self

    # -4*f*f + 3*5*f - 2 - 4*g * f
    # just sum of monomials
    def parse_polynomial(str)
      token = StringTokenizer.new(str)
      monomials = []

      token.skip_whitespace!

      return Polynomial.new unless token.can_read?
      
      first, last = 0, 0

      token.proceed! if token.peek.is_minus?
      
      while token.can_read?
        if token.peek.is_plus_minus?

          last = token.pos - 1
          is_minus = token.peek.is_minus?
          monomials << parse_monomial(str[first..last])

          first = token.pos
          first += 1 if token.peek.is_plus?
        end        

        token.proceed!
        token.skip_whitespace!
      end

      monomials << parse_monomial(str[first..-1]) if first < str.size
      
      Polynomial.new(monomials).compress!
    end

    def parse_monomial(str)
      # coef * x * coef* y * z
      token = StringTokenizer.new(str)
      powers = Hash.new(0)
      coef = 1.0

      token.skip_whitespace!

      return Monomial.new unless token.can_read?

      if token.peek.is_minus?
        coef *= -1
        token.proceed!
        token.skip_whitespace!
      end
      
      while token.can_read?

        if token.peek.is_number?
          coef *= token.read_numeric
        elsif token.peek.is_alpha?
          name = token.read_name
          powers[name] += 1

          if token.can_read? && token.peek.is_power?
            token.proceed!

            if token.can_read_digit?
              power = token.read_numeric
              powers[name] += power - 1
            else
              raise ParserException.new("Power expected at pos #{token.pos} in string [#{str}].")
            end
          end
          
        else
          raise ParserException.new("Unknown character at pos #{token.pos} in string [#{str}]. Variable name or constant expected")
        end

        token.skip_whitespace!

        if token.can_read?
          raise ParserException.new("Parsing error at pos #{token.pos} in string [#{str}]. Terminal symbol expexted.") unless token.peek.is_multiplication?
          token.proceed! # read terminal
          token.skip_whitespace!
        end
      end

      Monomial.new(powers, coef)
    end
  end
end
