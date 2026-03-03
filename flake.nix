{
  description = "Vektor flake";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs =
    {
      self,
      nixpkgs,
    }:
    let
      system = "x86_64-linux";
      pkgs = import nixpkgs { inherit system; };
    in
    {
      devShells.${system}.default = pkgs.mkShell {
        nativeBuildInputs = with pkgs; [
          gcc
          clang-tools
          lldb

          meson
          ninja
          pkg-config

          gtk4

          gdb
        ];

        shellHook = "";
      };
    };
}
