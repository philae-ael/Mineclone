{ pkgs ? import <nixpkgs> { } }:
let
  my-python = pkgs.python3;
  python-with-my-packages = my-python.withPackages (p: with p; [
    matplotlib
    numpy
    ipython
  ]);
in
pkgs.mkShell {
  buildInputs = [
    python-with-my-packages
    # other dependencies
  ];
  shellHook = ''
    PYTHONPATH=${python-with-my-packages}/${python-with-my-packages.sitePackages}
    # maybe set more env-vars
  '';
}

