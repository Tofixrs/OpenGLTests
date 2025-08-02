_: {
  perSystem = {
    pkgs,
    lib,
    ...
  }: {
    devShells = {
      default = pkgs.mkShell.override {inherit (pkgs.llvmPackages_21) stdenv;} rec {
        name = "shell";
        nativeBuildInputs = with pkgs; [
          cmake
          pkg-config
          ninja
          just
          clang-tools
        ];
        buildInputs = with pkgs; [
          wayland
          xorg.libX11
          xorg.libXrandr
          udev
          glfw
          mesa
          libGL
          libGLU
          glm
          imgui
          assimp
        ];

        LD_LIBRARY_PATH = lib.makeLibraryPath buildInputs;
      };
    };
  };
}
