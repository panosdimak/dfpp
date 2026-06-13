{
  description = "A pretty disk usage utility for your terminal, written in C++23";

  inputs = {
    nixpkgs.url = "github:nixos/nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };

  outputs = {
    self,
    nixpkgs,
    flake-utils,
  }:
    flake-utils.lib.eachDefaultSystem (
      system: let
        pkgs = nixpkgs.legacyPackages.${system};
      in {
        packages.default = pkgs.stdenv.mkDerivation {
          pname = "dfpp";
          version = "0.1.0";

          src = ./.;

          nativeBuildInputs = [pkgs.gcc15];

          enableParallelBuilding = true;

          installPhase = ''
            runHook preInstall
            install -Dm755 dfpp -t $out/bin
            runHook postInstall
          '';

          meta = {
            description = "Pretty disk usage utility for your terminal, written in C++23";
            homepage = "https://github.com/panosdimak/dfpp";
            mainProgram = "dfpp";
            platforms = pkgs.lib.platforms.linux;
          };
        };
      }
    );
}
