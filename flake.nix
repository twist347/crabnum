{
  description = "C++-23 environment";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixpkgs-unstable";

  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
    in {
      devShells.${system} = {

        default = pkgs.mkShell {
          name = "c23-gcc";
          packages = with pkgs; [ gcc15 cmake ninja gdb ];
          shellHook = ''
            export CC=gcc
            export CXX=g++
            echo "GCC: $(gcc --version | head -1)"
          '';
        };

#        clang = pkgs.mkShell {
#          name = "c23-clang";
#          packages = with pkgs; [ clang_22 cmake ninja lldb ];
#          shellHook = ''
#            export CC=clang
#            export CXX=clang++
#            echo "Clang: $(clang --version | head -1)"
#          '';
#        };

      };
    };
}