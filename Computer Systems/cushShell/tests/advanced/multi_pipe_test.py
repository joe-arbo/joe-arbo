from testutils import *

setup_tests()

# Test a really long pipeline
# Cat acts as the identity function for pipes

sendline("echo hi | cat | cat | cat | cat | cat | cat | cat")
expect_exact("hi\r\n", "multiple cats didn't work")

expect_prompt()

sendline("echo hi | cat | cat | cat | cat | cat | cat | cat | tr h b | cat")
expect_exact("bi\r\n", "multiple cats with a tr didn't work")

expect_prompt("Shell did not print expected prompt (2)")

# Test multiple substitutions through pipes

sendline("echo hello how are you | sed s/how/who/ | sed s/are/am/ | sed s/you/I/")
expect_exact("hello who am I\r\n", "Sed didn't work")

expect_prompt("Shell did not print expected prompt (3)")

# Reverse a string twice to get back the original
sendline("echo string | rev | rev ")
expect_exact("string\r\n", "Reverse twice didn't return string")

expect_prompt("Shell did not print expected prompt (4)")

test_success()
