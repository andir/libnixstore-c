{ stdenv, nixUnstable, fetchFromGitHub, boost, pkgconfig, cmake }:
let
  nix = nixUnstable.overrideAttrs (_: { src = fetchFromGitHub {
    owner = "andir";
    repo = "nix";
    rev = "2ac9aecfa673a1f182b68f55ddb38ab7e6cb1276";
    sha256 = "0fn86ck7fysym2fkyj6ycq0pnpiil51gd8axvdswybsswr0cggkf";
  }; });
in
stdenv.mkDerivation {
  name = "libnixstorec-0.0.1";
  src = ./.;

  nativeBuildInputs = [ pkgconfig cmake ];
  buildInputs = [ nix boost ];

  passthru = { inherit nix; };
}
