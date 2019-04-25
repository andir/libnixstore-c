with import <nixpkgs> {};
let
  nixstore-c = callPackage ./default.nix {};
in
runCommand "test-build" {
  buildInputs = [ nixstore-c ];
  nativeBuildInputs = [ gcc ];
} ''
  mkdir -p $out/bin
  set -ex
  cc ${./test.c} -I${nixstore-c}/include -L${nixstore-c}/lib -lnixstore-c -o $out/bin/test
''
