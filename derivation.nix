# https://discourse.nixos.org/t/how-to-use-a-nix-derivation-from-a-local-folder/5498/4

{ stdenv, lib, pkgs } :

stdenv.mkDerivation {
  pname = "persistent";
  version = "2.1.0";

  src = ./.;

  nativeBuildInputs = with pkgs; [ cmake ];

  enableParallelBuilding = true;

  # outputs = [ "out" ];

  cmakeFlags = [
    "-DPERSISTENT_CONFIG_INSTALL=ON"
  ];

  meta = with lib; {
    description = "A c++ struct persistene library";
    homepage = "https://github.com/r3dl3g/persistent";
    changelog = "https://github.com/r3dl3g/persistent/releases/tag/v${version}";
    license = licenses.mit;
    maintainers = with maintainers; [ r3dl3g ];
    platforms = with platforms; linux;
  };
}
