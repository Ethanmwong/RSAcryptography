# Public Key Cryptography

## Installations

To utilize this function, you will need to install:

- clang-15
- make

Before installation, make sure you update. To do this, type these commands into terminal.

```
sudo apt update
sudo apt upgrade -y
```

After, type this line into terminal to install git, make, clang-15, and gnuplot.

```
sudo apt install clang-15 clang git make gnuplot -y
```

## How to use `keygen.c`

1. In the directory where the files are, type `make all` or `make`. This will create an executable binary that the user can run.
2. To run the script, type `./keygen, followed by the acceptable commands.

## Acceptable commands

```
-b: specifies the minimum bits needed for the public modulus n.
-i: specifies the number of Miller-Rabin iterations for testing primes (default: 50)
-n pbfile: specifies the public key file (default: ss.pub)
-d pvfile: specifies the private key file (default: ss.priv)
-s: specifies random seed for random state initialization (default: time(NULL))
-v: enables verbose output
-h: Display a help message detailing program usage.

```

## How to use `encrypt.c`

1. In the directory where the files are, type `make all` or `make`. This will create an executable binary that the user can run.
2. To run the script, type `./encrypt, followed by the acceptable commands.

## Acceptable commands

```
-i: specifies the input file to encrypt (default: stdin)
-o: specifies the output file to encrypt (default: stdout)
-n: specifies the public key file (default: ss.pub)
-v: enables verbose output
-h: Display a help message detailing program usage.

```

## How to use `decrypt.c`

1. In the directory where the files are, type `make all` or `make`. This will create an executable binary that the user can run.
2. To run the script, type `./decrypt, followed by the acceptable commands.

## Acceptable commands

```
-i: specifies the input file to encrypt (default: stdin)
-o: specifies the output file to encrypt (default: stdout)
-n: specifies the public key file (default: ss.priv)
-v: enables verbose output
-h: Display a help message detailing program usage.

```

## Cleaning:

To clean the directory, type `make clean` to remove the executable binaries and the `.o` files from directory

## Errors:

None
