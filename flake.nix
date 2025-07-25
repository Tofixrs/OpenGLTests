{
  outputs = inputs @ {flake-parts, ...}:
    flake-parts.lib.mkFlake {inherit inputs;} (_: {
      systems = ["x86_64-linux"];
      imports = [
        {config._module.args._inputs = inputs // {inherit (inputs) self;};}
        ./parts
      ];
    });
  inputs = {
    nixpkgs.url = "github:NixOS/nixpkgs";
    flake-parts = {
      url = "github:hercules-ci/flake-parts";
      inputs.nixpkgs-lib.follows = "nixpkgs";
    };
  };
}
