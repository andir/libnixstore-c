{ stdenv, nixUnstable, fetchFromGitHub, boost, pkgconfig, cmake }:
let
  nix = nixUnstable.overrideAttrs (_: { src = fetchFromGitHub {
    owner = "andir";
    repo = "nix";
    rev = "3aefaac46e4833b029555242f90d37f0df80f02f";
    sha256 = "0aq0k4kyjcml5dgbhgzf0iv2wl2psy650w80g1d33pnzzv6gcdsr";
  }; });
in
stdenv.mkDerivation {
  name = "libnixstorec-0.0.1";
  src = ./.;

  nativeBuildInputs = [ pkgconfig cmake ];
  buildInputs = [ nix boost ];

  passthru = { inherit nix; };
}
