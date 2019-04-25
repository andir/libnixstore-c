{ stdenv, nixUnstable, fetchFromGitHub, boost, pkgconfig, cmake }:
let
  nix = nixUnstable.overrideAttrs (_: { src = fetchFromGitHub {
    owner = "andir";
    repo = "nix";
    rev = "41d103caf96a25c2f5ecc16b18b0d9162fe27c5b";
    sha256 = "1mkqddizckx511n309wpx7n7hg7yjz6929s7i1r079nsm89jxn28";
  }; });
in
stdenv.mkDerivation {
  name = "libnixstorec-0.0.1";
  src = ./.;

  nativeBuildInputs = [ pkgconfig cmake ];
  buildInputs = [ nix boost ];
}
