{ stdenv, nixUnstable, boost, pkgconfig, cmake }:
stdenv.mkDerivation {
  name = "libnixstorec-0.0.1";
  src = ./.;

  nativeBuildInputs = [ pkgconfig cmake ];
  buildInputs = [ nixUnstable boost ];
}
