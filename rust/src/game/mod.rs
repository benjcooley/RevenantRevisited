// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                mod.rs - Screen flow and session ownership             *
// *************************************************************************

pub mod session;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum MenuOrigin {
    Title,
    Pause,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Destination {
    Title,
    Exit,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Screen {
    Title,
    Loading,
    Play,
    Pause,
    Options(MenuOrigin),
    Confirm(Destination),
    Death,
    Exit,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub enum Action {
    NewGame,
    Loaded,
    Pause,
    Resume,
    Options,
    Back,
    ReturnToTitle,
    Quit,
    Confirm,
    PlayerDied,
    VolumeDown,
    VolumeUp,
    ToggleSound,
    ToggleShadows,
    ApplyOptions,
}

/// Work the application must perform after an accepted transition.
#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum Effect {
    #[default]
    None,
    StartSession,
    EndSession,
    ApplyOptions,
    Quit,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct Settings {
    pub volume: u8,
    pub muted: bool,
    pub shadows: bool,
}
impl Default for Settings {
    fn default() -> Self {
        Self {
            volume: 70,
            muted: false,
            shadows: true,
        }
    }
}

/// Named intents replace C++ next-screen pointers. Simulation belongs only to
/// Play; loading, dialogs and options never advance the session's clock.
pub struct Flow {
    screen: Screen,
    settings: Settings,
    draft: Settings,
    error: Option<String>,
}
impl Flow {
    pub fn new(mut settings: Settings) -> Self {
        settings.volume = settings.volume.min(100);
        Self {
            screen: Screen::Title,
            settings,
            draft: settings,
            error: None,
        }
    }
    pub fn screen(&self) -> Screen {
        self.screen
    }
    pub fn settings(&self) -> Settings {
        self.settings
    }
    pub fn options(&self) -> Settings {
        self.draft
    }
    pub fn error(&self) -> Option<&str> {
        self.error.as_deref()
    }
    pub fn simulates(&self) -> bool {
        self.screen == Screen::Play
    }

    pub fn load_failed(&mut self, message: String) -> Effect {
        if self.screen != Screen::Loading {
            return Effect::None;
        }
        self.error = Some(message);
        self.screen = Screen::Title;
        Effect::EndSession
    }

    pub fn dispatch(&mut self, action: Action) -> Effect {
        use Action as A;
        use Screen as S;
        match (self.screen, action) {
            (S::Title | S::Death, A::NewGame) => {
                self.error = None;
                self.screen = S::Loading;
                Effect::StartSession
            }
            (S::Loading, A::Loaded) => {
                self.screen = S::Play;
                Effect::None
            }
            (S::Play, A::Pause | A::Back) => {
                self.screen = S::Pause;
                Effect::None
            }
            (S::Pause, A::Resume | A::Back) => {
                self.screen = S::Play;
                Effect::None
            }
            (S::Title | S::Pause, A::Options) => {
                let origin = if self.screen == S::Title {
                    MenuOrigin::Title
                } else {
                    MenuOrigin::Pause
                };
                self.draft = self.settings;
                self.screen = S::Options(origin);
                Effect::None
            }
            (S::Options(origin), A::ApplyOptions | A::Back) => {
                let effect = if action == A::ApplyOptions {
                    self.settings = self.draft;
                    Effect::ApplyOptions
                } else {
                    Effect::None
                };
                self.screen = match origin {
                    MenuOrigin::Title => S::Title,
                    MenuOrigin::Pause => S::Pause,
                };
                effect
            }
            (S::Options(_), A::VolumeDown) => {
                self.draft.volume = self.draft.volume.saturating_sub(10);
                Effect::None
            }
            (S::Options(_), A::VolumeUp) => {
                self.draft.volume = self.draft.volume.saturating_add(10).min(100);
                Effect::None
            }
            (S::Options(_), A::ToggleSound) => {
                self.draft.muted = !self.draft.muted;
                Effect::None
            }
            (S::Play, A::ToggleSound) => {
                self.settings.muted = !self.settings.muted;
                Effect::ApplyOptions
            }
            (S::Options(_), A::ToggleShadows) => {
                self.draft.shadows = !self.draft.shadows;
                Effect::None
            }
            (S::Pause, A::ReturnToTitle) => {
                self.screen = S::Confirm(Destination::Title);
                Effect::None
            }
            (S::Pause, A::Quit) => {
                self.screen = S::Confirm(Destination::Exit);
                Effect::None
            }
            (S::Confirm(_), A::Back) => {
                self.screen = S::Pause;
                Effect::None
            }
            (S::Confirm(Destination::Title), A::Confirm)
            | (S::Death, A::ReturnToTitle)
            | (S::Loading, A::Back) => {
                self.screen = S::Title;
                Effect::EndSession
            }
            (S::Title, A::Quit | A::Back)
            | (S::Death, A::Quit)
            | (S::Confirm(Destination::Exit), A::Confirm) => {
                self.screen = S::Exit;
                Effect::Quit
            }
            (S::Play, A::PlayerDied) => {
                // gameflow.cpp ends the session when leaving play for death.
                self.screen = S::Death;
                Effect::EndSession
            }
            _ => Effect::None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn pause_options_and_confirmation_do_not_simulate_or_destroy_the_session() {
        let mut flow = Flow::new(Settings::default());
        assert_eq!(flow.dispatch(Action::Loaded), Effect::None);
        assert_eq!(flow.screen(), Screen::Title);
        assert_eq!(flow.dispatch(Action::NewGame), Effect::StartSession);
        assert!(!flow.simulates());
        flow.dispatch(Action::Loaded);
        assert!(flow.simulates());
        for action in [
            Action::Pause,
            Action::Options,
            Action::Back,
            Action::ReturnToTitle,
            Action::Back,
        ] {
            assert_eq!(flow.dispatch(action), Effect::None);
            assert!(!flow.simulates());
        }
        flow.dispatch(Action::Resume);
        assert!(flow.simulates());
        flow.dispatch(Action::Pause);
        flow.dispatch(Action::ReturnToTitle);
        assert_eq!(flow.dispatch(Action::Confirm), Effect::EndSession);
        assert_eq!(flow.screen(), Screen::Title);
    }
    #[test]
    fn loading_can_fail_cancel_and_retry_without_entering_play() {
        let mut flow = Flow::new(Settings::default());
        flow.dispatch(Action::NewGame);
        assert_eq!(
            flow.load_failed("missing sector".into()),
            Effect::EndSession
        );
        assert_eq!(flow.error(), Some("missing sector"));
        assert_eq!(flow.dispatch(Action::NewGame), Effect::StartSession);
        assert_eq!(flow.error(), None);
        assert_eq!(flow.dispatch(Action::Back), Effect::EndSession);
        flow.dispatch(Action::Loaded); // A late completion cannot reopen a cancelled game.
        assert_eq!(flow.screen(), Screen::Title);
    }
    #[test]
    fn options_apply_or_cancel_and_return_to_their_origin() {
        let original = Settings::default();
        let mut flow = Flow::new(original);
        flow.dispatch(Action::Options);
        flow.dispatch(Action::ToggleSound);
        flow.dispatch(Action::VolumeDown);
        assert_eq!(flow.settings(), original);
        flow.dispatch(Action::Back);
        assert_eq!(flow.settings(), original);
        flow.dispatch(Action::Options);
        for _ in 0..20 {
            flow.dispatch(Action::VolumeUp);
        }
        assert_eq!(flow.options().volume, 100);
        flow.dispatch(Action::ToggleShadows);
        assert_eq!(flow.dispatch(Action::ApplyOptions), Effect::ApplyOptions);
        assert_eq!(flow.settings().volume, 100);
        assert!(!flow.settings().shadows);
        assert_eq!(flow.screen(), Screen::Title);
    }
    #[test]
    fn death_ends_the_session_and_restart_begins_a_fresh_one() {
        let mut flow = Flow::new(Settings::default());
        flow.dispatch(Action::NewGame);
        flow.dispatch(Action::Loaded);
        assert_eq!(flow.dispatch(Action::PlayerDied), Effect::EndSession);
        assert_eq!(flow.screen(), Screen::Death);
        assert_eq!(flow.dispatch(Action::NewGame), Effect::StartSession);
    }

    #[test]
    fn play_mute_shortcut_survives_cancelling_the_options_draft() {
        let mut flow = Flow::new(Settings::default());
        flow.dispatch(Action::NewGame);
        flow.dispatch(Action::Loaded);
        assert_eq!(flow.dispatch(Action::ToggleSound), Effect::ApplyOptions);
        assert!(flow.simulates());
        assert!(flow.settings().muted);
        flow.dispatch(Action::Pause);
        flow.dispatch(Action::Options);
        assert!(flow.options().muted);
        flow.dispatch(Action::ToggleSound);
        flow.dispatch(Action::Back);
        assert!(flow.settings().muted);
    }
}
