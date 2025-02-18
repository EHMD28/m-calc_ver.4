# M-Calculator Version 4

A fully featured CLI scientific calculator. **THIS PROJECT IS A WIP**

## Usage

Start `mcalc4` by typing in `mcalc4` command with no arguments. If there
are arguments, they will be evaluted without the application actually
starting. Once the application is started, the user will be prompted to 
enter an input until they type `exit`, at which point the program will
exit. 

### Demo
```
$ mcaclc4
mcalc4> 2 + 4
2 + 4 = 6
mcalc4> sin(pi/4)
sin(pi/4) = 0.7071
mcalc4> settings ANGLE RAD
sin(45) = 0.7071
mcalc4> set x = 4
x = 4
mcalc4> print x
x = 4
mcalc4> print "history.txt"
Writing history to "history.txt"
mcalc4> exit
$ 
```

## Expressions

When an input is given to `mcalc4` with a first word that isn't one of the 
designated commands, the input will be evaluates as a mathematical expression.
These expressions can include all of the basic arithmetic operators (`+-*/^`),
functions such as `sin`, `cos`, `tan`, `arcsin`, `arccos`, and `arctan`, and
constants such as `pi` and `e`. All of these values are case insinsitive,
so don't worry about capitalization. **It is important to note that π is not
recognized by the calculator, alongside all other Unicode characters.**

## Variables
The syntax for declaring variables in `mcacl4` is `mcalc4> set {var_name} = 4`.
Variables can be accessed by using them in expressions such as `2 * x` or
`x ^ 2`. Note that `e` cannot be used as a variable name because it is a
constant.

## Settings

## Print
