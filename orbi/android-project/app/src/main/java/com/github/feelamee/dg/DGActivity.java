package com.github.feelamee.dg;

import org.libsdl.app.SDLActivity;

public class DGActivity extends SDLActivity {
    @Override
    protected String getMainFunction() {
        return "main";
    }

    @Override
    protected String[] getLibraries() {
        return new String[] {
                "c++_shared",
                "SDL3",
                "engine",
                "orbi",
        };
    }
}
