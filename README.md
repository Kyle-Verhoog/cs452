# Benjamin Zhao and Kyle Verhoog's CS452 Kernel Monorepo


# NOTE TO TAs:
**Our TC1 submission is in a branch: milestone1**

**Our kernel 3 submission is in a branch: kernel3**

**The kernel 3 doc is located at docs/k3.pdf.**

**Our kernel 2 submission is in a branch: kernel2**

**The kernel 2 doc is located at docs/k2.pdf.**

**Our kernel 1 submission is in a branch: kernel1**

**The kernel 1 doc is located at docs/k1.pdf.**


## Documentation 
The documentation is written in Markdown and generated using `pandoc`. To
generate the documentation run `make docs`.

## Developing 

### Debugging
To enable debugging messages and `KASSERT`s run `make debug`.

### Building 
To build the kernel run `make`. To build and copy to the tftp server, run `make
copy`.

### Formatting 
Run `make format` to run the code formatter on the code of the project. Note
that is destructive!

### Testing

#### Unit Tests (for data structures)
To run the unit tests (x86) run `make test`.

#### Task tests (for kernel testing)
There are tests to test task creation and syscalls and more. To run these tests
run `make ktest`.
