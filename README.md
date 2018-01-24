# Benjamin Zhao and Kyle Verhoog's CS452 Kernel Monorepo


## Documentation 
The documentation is written in Markdown and generated using `pandoc`. To
generate the documentation run `make docs`.

## Developing 
### Building 
To build the kernel run `make`. To build and copy to the tftp server, run `make
copy`.

### Formatting 
Run `make format` to run the code formatter on the code of the project. Note
that is destructive!

### Testing To run the unit tests (x86) run `make test`.
