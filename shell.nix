{ pkgs ? import <nixpkgs> {} }:
pkgs.mkShell {
  buildInputs = with pkgs; [
    conan cmake pkg-config
    buildPackages.gcc

    # system libs
    # I dont use all that lol, but conan ant all of them
    xorg.libX11 xorg.libfontenc libGL xorg.libICE xorg.libSM xorg.libXau xorg.libXaw xorg.libXcomposite xorg.libXcursor
    xorg.libXdamage xorg.libXdmcp xorg.libXft xorg.libXtst xorg.libXinerama xorg.libxkbfile xorg.libXrandr
    xorg.libXres xorg.libXScrnSaver xorg.libXvMC xorg.xtrans xorg.xcbutil.dev xorg.xcbutilcursor.dev xorg.xcbutilerrors 
    xorg.xcbutilkeysyms.dev xorg.xcbutilrenderutil.dev xorg.xcbutilwm.dev xorg.libXxf86vm xorg.libXv xkeyboard_config 
    xorg.libXpm libglvnd.dev mesa_drivers mesa_glu.dev mesa.dev xlibs.libxcb.dev xlibs.xcbutilimage.dev xlibs.xcbutilwm.dev
    xlibs.xorgserver.dev xorg.libpthreadstubs xorg.libX11.dev xorg.libxcb.dev xorg.libXext.dev xorg.libXi.dev 
    xorg.xcbproto xorg.xorgproto





    # Language servers
    cmake-language-server
    clang-tools
    ccls
    unzip zip curl

  ];
}
