import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0

Page {
    id: page
    allowedOrientations: Orientation.All

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                //: Page title
                title: qsTr("Settings")
            }

            TextSwitch {
                text: qsTr("Enable logging")
                checked: settings.enableLogging
                automaticCheck: false

                onClicked: {
                    if (checked) {
                        const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                            messages: [
                                //: Full effect here means that the setting will not propagate to all parts of the app so some parts will still collect logs
                                qsTr("This will disable log collection, you should restart the app afterwards so it takes full effect."),
                                "<strong>" + qsTr("After disabling logging all previous logs will be deleted to save space.") + "</strong>",
                                "<strong>" + qsTr("If you have not exported the logs anywhere you should do so before disabling logging.") + "</strong>",
                            ]
                        });
                        dialog.accepted.connect(function() {
                            settings.enableLogging = false;
                            logger.deleteLogs();
                        });
                    } else {
                        const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                            messages: [
                                qsTr("If you have problems with the app, enable this option to collect logs which you can then send to the developer."),
                                //: Full effect here means that the setting will not propagate to all parts of the app so some parts will not collect logs until restart
                                "<strong>" + qsTr("You need to restart the app for the logging to take full effect.") + "</strong>",
                            ]
                        });
                        dialog.accepted.connect(function() {
                            settings.enableLogging = true;
                        });
                    }
                }
            }

            TextSwitch {
                text: qsTr("Conversation length: %1").arg(settings.conversationLength)
                checked: true
                automaticCheck: false

                onClicked: {
                    const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                        messages: [
                            qsTr("Configure the length of the conversation that is sent to the ChatGPT model."),
                            qsTr("To maintain context, the AI model needs to see the previous messages as well, this configures how many past messages are sent."),
                            qsTr("Note that previous messages count towards the token limit of the AI model which is currently %1 tokens.").arg(4096),
                        ],
                        intFieldVisible: true,
                        intFieldDescription: qsTr("Conversation length (number of messages)"),
                        intValue: settings.conversationLength,
                    });

                    dialog.accepted.connect(function() {
                        const value = dialog.intValue;
                        settings.conversationLength = value;
                    });
                }
            }

            TextSwitch {
                //: Read the description at https://platform.openai.com/docs/api-reference/chat/create#chat/create-temperature to understand what the temperature here means. I recommend keeping it untranslated if the translation is not used in the AI community in your language.
                text: qsTr("Sampling temperature: %1").arg(settings.temperature)
                checked: true
                automaticCheck: false

                onClicked: {
                    const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                        messages: [
                            qsTr("What sampling temperature to use."),
                            qsTr("Higher values like 0.8 will make the output more random, while lower values like 0.2 will make it more focused and deterministic."),
                            qsTr("It's generally recommended to alter this setting or top percentage, but not both."),
                        ],
                        sliderVisible: true,
                        sliderMax: 2,
                        sliderMin: 0,
                        sliderStep: 0.1,
                        //: Read the description at https://platform.openai.com/docs/api-reference/chat/create#chat/create-temperature to understand what the temperature here means. I recommend keeping it untranslated if the translation is not used in the AI community in your language.
                        sliderDescription: qsTr("Sampling temperature"),
                        sliderValue: settings.temperature,
                    });

                    dialog.accepted.connect(function() {
                        const value = dialog.sliderValue;
                        settings.temperature = value;
                    });
                }
            }

            TextSwitch {
                //: Read the description at https://platform.openai.com/docs/api-reference/chat/create#chat/create-top_p to understand what it means. I recommend keeping it untranslated if the translation is not used in the AI community in your language.
                text: qsTr("Top percentage: %1").arg(settings.topPercentage)
                checked: true
                automaticCheck: false

                onClicked: {
                    const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                        messages: [
                            qsTr("An alternative to sampling with temperature, called nucleus sampling, where the model considers the results of the tokens with top percentage probability mass."),
                            qsTr("So 0.1 means only the tokens comprising the top 10% probability mass are considered."),
                            qsTr("It's generally recommended to alter this setting or sampling temperature, but not both."),
                        ],
                        sliderVisible: true,
                        sliderMax: 1,
                        sliderMin: 0,
                        sliderStep: 0.01,
                        //: Read the description at https://platform.openai.com/docs/api-reference/chat/create#chat/create-top_p to understand what it means. I recommend keeping it untranslated if the translation is not used in the AI community in your language.
                        sliderDescription: qsTr("Top percentage"),
                        sliderValue: settings.topPercentage,
                    });

                    dialog.accepted.connect(function() {
                        const value = dialog.sliderValue;
                        settings.topPercentage = value;
                    });
                }
            }

            TextSwitch {
                //: Read the description at https://platform.openai.com/docs/api-reference/chat/create#chat/create-presence_penalty to understand what it means. I recommend keeping it untranslated if the translation is not used in the AI community in your language.
                text: qsTr("Presence penalty: %1").arg(settings.presencePenalty)
                checked: true
                automaticCheck: false

                onClicked: {
                    const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                        messages: [
                            qsTr("Positive values penalize new tokens based on whether they appear in the text so far, increasing the model's likelihood to talk about new topics."),
                        ],
                        sliderVisible: true,
                        sliderMax: 2,
                        sliderMin: -2,
                        sliderStep: 0.1,
                        //: Read the description at https://platform.openai.com/docs/api-reference/chat/create#chat/create-presence_penalty to understand what it means. I recommend keeping it untranslated if the translation is not used in the AI community in your language.
                        sliderDescription: qsTr("Presence penalty"),
                        sliderValue: settings.presencePenalty,
                    });

                    dialog.accepted.connect(function() {
                        const value = dialog.sliderValue;
                        settings.presencePenalty = value;
                    });
                }
            }

            TextSwitch {
                //: Read the description at https://platform.openai.com/docs/api-reference/chat/create#chat/create-frequency_penalty to understand what it means. I recommend keeping it untranslated if the translation is not used in the AI community in your language.
                text: qsTr("Frequency penalty: %1").arg(settings.frequencyPenalty)
                checked: true
                automaticCheck: false

                onClicked: {
                    const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                        messages: [
                            qsTr("Positive values penalize new tokens based on their existing frequency in the text so far, decreasing the model's likelihood to repeat the same line verbatim."),
                        ],
                        sliderVisible: true,
                        sliderMax: 2,
                        sliderMin: -2,
                        sliderStep: 0.1,
                        //: Read the description at https://platform.openai.com/docs/api-reference/chat/create#chat/create-frequency_penalty to understand what it means. I recommend keeping it untranslated if the translation is not used in the AI community in your language.
                        sliderDescription: qsTr("Presence penalty"),
                        sliderValue: settings.frequencyPenalty,
                    });

                    dialog.accepted.connect(function() {
                        const value = dialog.sliderValue;
                        settings.frequencyPenalty = value;
                    });
                }
            }
        }
    }

    Component.onCompleted: {
        logger.debug("Navigated to SettingsPage.qml");
    }
}
