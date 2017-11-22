from spec.environment import *

with describe(RbString):
    with before.each:
        self.context = RbContext(RUBY_HELPER_PATH)
        self.RubyHelper = self.context.rb_eval("RubyHelper")

    with after.each:
        RbContext.finalize()

    with it('exists'):
        assert_that(RbString, is_(not_none()))

    with it('is possible to convert from a raw python string'):
        rb_string = to_ruby("sample")
        assert_that(rb_string, instance_of(RbString))
        assert_that(rb_string, is_(equal_to('foo')))

    with it('is comparable to a raw Python string'):
        rb_string = self.context.rb_eval('""')
        assert_that(rb_string, is_(equal_to('')))
        rb_string = self.context.rb_eval('"foo"')
        assert_that(rb_string, is_(equal_to('foo')))

    with it('is possible to exfiltrate an RbString reference from Ruby via the RbContext'):
        rb_string = self.context.rb_eval('"foo"')
        assert_that(rb_string, instance_of(RbString))
        assert_that(rb_string, is_(equal_to('foo')))

    with context('Ruby string interface'):
        with it('behaves properly for array referencing'):
            str1 = self.context.rb_eval('"FooBar"')
            assert_that(str1[0], is_(equal_to('F')))
            assert_that(str1[-1], is_(equal_to('r')))
            assert_that(str1[6], is_none())
            assert_that(str1[-7], is_none())

            assert_that(str1[0,3], is_(equal_to("Foo")))
            assert_that(str1[-3,3], is_(equal_to("Bar")))
            assert_that(str1[6,2], is_(equal_to("")))
            assert_that(str1[-7,10], is_none())

            # TODO: More tests

        with it('behaves properly for array assignment'):
            str1 = self.context.rb_eval('"FooBar"')
            str1[0] = 'A'
            assert_that(str1, is_(equal_to('AooBar')))

            str1[-1] = 'B'
            assert_that(str1, is_(equal_to('AooBaB')))
            # assert_that(calling(lambda s: s[-7] = 'xyz').with_args(str1), raises(RubythonError))
            assert_that(str1, is_(equal_to('AooBaB')))
            str1[0] = 'ABC'
            assert_that(str1, is_(equal_to('ABCooBaB')))

            str2 = self.context.rb_eval('"FooBar"')
            str2[0, 3] = 'A'
            assert_that(str2, is_(equal_to('ABar')))
            str2[0] = 'Foo'
            assert_that(str2, is_(equal_to('FooBar')))
            str2[-3,3] = 'Foo'
            assert_that(str2, is_(equal_to('FooFoo')))
            # assert_that(calling(lambda s: s[7,3] = 'Bar').with_args(str2), raises(RubythonError))
            # assert_that(calling(lambda s: s[-7,3] = 'Bar').with_args(str2), raises(RubythonError))

            # TODO: Range syntax support

        with it('should behave properly for `<=>` comparison'):
            str1 = self.context.rb_eval('"abcdef"')
            str2 = self.context.rb_eval('"abcde"')
            assert_that(str1.send('<=>', str2), is_(equal_to(1)))
            assert_that(cmp(str1, str2), is_(equal_to(1)))
            assert_that(str1.send('<=>', str1), is_(equal_to(0)))
            assert_that(cmp(str1, str1), is_(equal_to(0)))
            assert_that(str2.send('<=>', str1), is_(equal_to(-1)))
            assert_that(cmp(str2, str1), is_(equal_to(-1)))
            # TODO: Other stuff support

        with it('should behave properly for `==` comparison'):
            sym1 = self.context.rb_eval(':foo')
            str1 = self.context.rb_eval('"foo"')
            assert_that(not(str1.send('==', sym1)))
            assert_that(not(str1 == sym1))
            assert_that(str1.send('==', str1))
            assert_that(str1 == str1)

        with it('should behave properly for lshift assignment'):
            str1 = self.context.rb_eval('"world"')
            assert_that(str1.send('<<', 33), is_(equal_to('world!')))

        # TODO: Other string tests

        with it('should behave properly for `chomp`'):
            str1 = self.context.rb_eval('"hello"')
            str2 = self.context.rb_eval('"hello\n"')
            str3 = self.context.rb_eval('"hello!"')
            assert_that(str1.chomp("\n"), is_(equal_to('hello')))
            assert_that(str2.chomp("\n"), is_(equal_to('hello')))
            self.context.rb_eval('save = $/')

            self.context.rb_eval('$/ = "\n"')
            assert_that(str1.chomp(), is_(equal_to('hello')))
            assert_that(str2.chomp(), is_(equal_to('hello')))

            self.context.rb_eval('$/ = "!"')
            assert_that(str1.chomp(), is_(equal_to('hello')))
            assert_that(str3.chomp(), is_(equal_to('hello')))
            self.context.rb_eval('$/ = save')

        with it('should behave properly for `chop`'):
            assert_that(self.context.rb_eval('"hello"').chop(), is_(equal_to('hell')))
            assert_that(self.context.rb_eval('"hello\r\n"').chop(), is_(equal_to('hello')))
            assert_that(self.context.rb_eval('"hello\n\r"').chop(), is_(equal_to('hello\n')))
            assert_that(self.context.rb_eval('"\r\n"').chop(), is_(equal_to('')))
            assert_that(self.context.rb_eval('""').chop(), is_(equal_to('')))

        with it('should behave properly for `clone`'):
            for taint in [False, True]:
                for frozen in [False, True]:
                    a = self.context.rb_eval('"Cool"')
                    if taint:
                      a.taint()
                    if frozen:
                      a.freeze()
                    b = a.clone()

                    assert_that(a, is_(equal_to(b)))
                    assert_that(a, not_(same_instance(b)))
                    assert_that(a.send('frozen?'), is_(equal_to(b.send('frozen?'))))
                    assert_that(a.send('tainted?'), is_(equal_to(b.send('tainted?'))))

        with it('should behave properly for `concat`'):
            str1 = self.context.rb_eval('"world"')
            assert_that(str1.concat(33), is_(equal_to('world!')))
            str1 = self.context.rb_eval('"world"')
            assert_that(str1.concat('!'), is_(equal_to('world!')))

        with it('should behave properly for `count`'):
            str1 = self.context.rb_eval('"hello world"')
            assert_that(str1.count('lo'), is_(equal_to(5)))
            assert_that(str1.count('lo', 'o'), is_(equal_to(2)))
            assert_that(str1.count('hello', '^l'), is_(equal_to(4)))
            assert_that(str1.count('ej-m'), is_(equal_to(4)))
            str2 = self.context.rb_eval('"y"')
            assert_that(str2.count('a\\-z'), is_(equal_to(0)))
