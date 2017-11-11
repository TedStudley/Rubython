from spec.python.environment import *

with description('rubython'):
    with it('should exist'):
        assert_that(rubython.__version__, is_(not_none()))
