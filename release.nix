{ nixpkgs ? <nixpkgs> }:
let
  pkgs = import nixpkgs {};
in
{
  libnixstore-c = pkgs.callPackage ./default.nix {};
  test = import ./test.nix;
}
