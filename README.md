decBRL-cpp
==========

Decentralised Bayesian Reinforcement Learning Library, written in C++

The API documentation for this library can be found here: http://lteacy.github.com/decBRL-cpp/

Dependencies
============
This software currently depends on the random module of Boost (see http://www.boost.org/), and the maxsum-cpp library (see https://github.com/lteacy/maxsum-cpp).

doxygen is required to compile documentation in html. In addition, LaTeX is required to generate forumla's included in the html, and to generate a pdf version of the documentation.

Installation
============
Install CMake (see http://www.cmake.org/) and follow the general instructions for building CMake projects for your platform.
Install the maxsum-cpp library. At the moment the build assumes that both decBRL-cpp and maxsum-cpp are located in the same directory.
Now build decBRL-cpp using cmake. For example, on most unix-like platforms (including Mac OS X) it is usually sufficient to run the following commands:

    cd maxsum-cpp
    cmake .
    make
    cd ../decBRL-cpp
    cmake .
    make

Optionally, to run unit tests:

    ctest .

And to build documentation:

    make doc

If your platform has multiple cores, both make and ctest can run in parallel, by specifying the number of cores on the command line.
For example, on a 4 core machine, run:

    make -j 4
    ctest -j 4

Known Issues
============
This library is not complete yet!! The initial phase of development is still underway, so there is nothing worthing linking to just yet. 

License
=======
With the exception of the files contained under the include/polygamma and src/polygamma subdirectories, all files within this project are distributed under the BSD License stated below. The files contained within the polygamma subdirectories are due to John Burkardt, and are distributed under the LGPL license. A copy of this license is stated in the README files contained in each of the polygamma subdirectories. All other code within the library may be used without adhering to the terms of the LGPL license, provided the LGPL license code is replaced with some other implementation of the digamma and trigamma functions. 

In the text below, all references to 'this software' refers to the files contained within this project, excluding only the LGPL licensed code contained under the polygamma subdirectories.

See also:
* John Burkardt's website http://people.sc.fsu.edu/~jburkardt/
* The LGPL license https://github.com/lteacy/decBRL-cpp/blob/learning/src/polygamma/README.txt

BSD License
-----------
Copyright © 2012 Luke Teacy. All Rights Reserved.

Redistribution and use of this software in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
3. The name of the author may not be used to endorse or promote products derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY LUKE TEACY "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
