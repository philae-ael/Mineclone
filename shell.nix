{ pkgs ? import <nixpkgs> { } }:
pkgs.mkShell.override
{
  # stdenv = pkgs.llvmPackages_latest.stdenv;
  stdenv = pkgs.gcc11Stdenv;
}
{
  buildInputs = with pkgs; [
    # system libs
    # I dont use all that lol, but conan want all of them
    xorg.libX11
    xorg.libfontenc
    libGL
    xorg.libICE
    xorg.libSM
    xorg.libXau
    xorg.libXaw
    xorg.libXcomposite
    xorg.libXcursor
    xorg.libXdamage
    xorg.libXdmcp
    xorg.libXft
    xorg.libXtst
    xorg.libXinerama
    xorg.libxkbfile
    xorg.libXrandr
    xorg.libXres
    xorg.libXScrnSaver
    xorg.libXvMC
    xorg.xtrans
    xorg.xcbutil.dev
    xorg.xcbutilcursor.dev
    xorg.xcbutilerrors
    xorg.xcbutilkeysyms.dev
    xorg.xcbutilrenderutil.dev
    xorg.xcbutilwm.dev
    xorg.libXxf86vm
    xorg.libXv
    xkeyboard_config
    xorg.libXpm
    libglvnd.dev
    mesa_drivers
    mesa_glu.dev
    mesa.dev
    xlibs.libxcb.dev
    xlibs.xcbutilimage.dev
    xlibs.xcbutilwm.dev
    xlibs.xorgserver.dev
    xorg.libpthreadstubs
    xorg.libX11.dev
    xorg.libxcb.dev
    xorg.libXext.dev
    xorg.libXi.dev
    xorg.xcbproto
    xorg.xorgproto
  ];

  nativeBuildInputs = with pkgs; [
    # I put it here so that its path is well set
    clang-tools
    conan
    cmake
    pkg-config
    gdb
    renderdoc

    # Language servers
    cmake-language-server
    unzip
    zip
    curl

  ];
}
