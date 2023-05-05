import QtQuick 2.0
import Sailfish.Silica 1.0

import cz.chrastecky.chatgpt 1.0

Page {
    property var doAfterLoad: []
    property var modelsList: []

    id: page
    allowedOrientations: Orientation.All

    function doAfterLoadOrNow(callable) {
        if (page.status === PageStatus.Active) {
            callable();
        } else {
            doAfterLoad.push(callable);
        }
    }

    LocaleHelper {
        id: locale
    }

    SecretsHandler {
        id: secrets
    }

    ChatGptClient {
        id: chatGpt

        onApiKeyChecked: {
            if (isValid) {
                secrets.setApiKey(apiKey);
                loader.running = false;
            } else {
                secrets.removeApiKey();
                pageStack.replace("InitialChecksPage.qml");
            }
        }

        onModelsListReceived: {
            modelsList = models;
        }
    }

    BusyLabel {
        id: loader
        text: qsTr("Checking api key")
        running: false
    }

    BusyLabel {
        id: loader2
        text: qsTr("Loading available AI models")
        running: modelsList.length === 0
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height
        visible: !loader.running && !loader2.running

        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                //: Page title
                title: qsTr("Settings")
            }

            PullDownMenu {
                MenuItem {
                    text: qsTr("Change api key")
                    onClicked: {
                        const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                            messages: [
                                qsTr("Here you can change the api key in use for this app."),
                                qsTr("The key will be validated before being accepted."),
                            ],
                            stringFieldVisible: true,
                            stringFieldDescription: qsTr('API key'),
                            stringFieldValue: secrets.apiKey(),
                        });
                        dialog.accepted.connect(function() {
                            loader.running = true;
                            chatGpt.checkApiKey(dialog.stringFieldValue);
                        });
                    }
                }
            }

            TextSwitch {
                text: qsTr("Language") + ": " + (settings.language
                                                 ? locale.languageName(settings.language)
                                                 //: As in automatically selected language
                                                 : qsTr("Automatic"))
                checked: true
                automaticCheck: false

                onClicked: {
                    const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                        messages: [
                            qsTr("Choose a display language for the app:"),
                        ],
                        comboBoxVisible: true,
                        comboBoxValue: settings.language,
                        comboBoxDescription: qsTr("Language"),
                        comboBoxOptions: (function() {
                            var result = [
                                {
                                    //: As in automatically selected language
                                    name: qsTr("Automatic"),
                                    value: "",
                                },
                            ];
                            for (var i in languages) {
                                if (!languages.hasOwnProperty(i)) {
                                    continue;
                                }

                                result.push({
                                    name: locale.languageName(languages[i]),
                                    value: languages[i],
                                });
                            }

                            return result;
                        })(),
                    });
                    dialog.accepted.connect(function() {
                        settings.language = dialog.comboBoxResult;
                    });
                }
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
                text: qsTr("AI model: %1").arg(settings.aiModel)
                checked: true
                automaticCheck: false

                onClicked: {
                    const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                        messages: [
                            qsTr("Configure the AI model used for generating responses."),
                            qsTr("Newer models are generally better than the old ones (for example %1 is better than %2).").arg('gpt-4').arg('gpt-3.5-turbo'),
                            qsTr("If you choose a base model (like %1) it will automatically use the latest model of that series. You can avoid this by choosing specific version (like %2).").arg('gpt-3.5-turbo').arg('gpt-3.5-turbo-0301')
                        ],
                        comboBoxVisible: true,
                        comboBoxValue: settings.aiModel,
                        comboBoxDescription: qsTr("AI model"),
                        comboBoxOptions: modelsList.map(function(item) {
                            return {name: item, value: item};
                        }),
                    });

                    dialog.accepted.connect(function() {
                        settings.aiModel = dialog.comboBoxResult;
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

            TextSwitch {
                text: qsTr("Custom system message")
                checked: settings.systemMessage
                automaticCheck: false

                onClicked: {
                    const dialog = pageStack.push("ConfirmSettingDialog.qml", {
                        messages: [
                            qsTr("You can set a system message that is sent as an instruction to GPT before each chat begins."),
                            qsTr("For example you can write 'You are a helpful assistant' and the AI should try to follow your instructions."),
                            qsTr("Note: The current version of GPT doesn't pay strong attention to the system message, it should be mitigated when the model is updated."),
                            //: The %1 gets replaced by the translation for 'Conversation length'
                            qsTr("Note: The system message counts towards your '%1' limit.").arg(qsTr('Conversation length')),
                        ],
                        stringFieldVisible: true,
                        stringFieldDescription: qsTr('Custom system message'),
                        stringFieldValue: settings.systemMessage,
                    });

                    dialog.accepted.connect(function() {
                        settings.systemMessage = dialog.stringFieldValue;
                    });
                }
            }
        }
    }

    onStatusChanged: {
        if (status === PageStatus.Active) {
            while (doAfterLoad.length) {
                const callable = doAfterLoad.shift();
                callable();
            }
        }
    }

    Component.onCompleted: {
        chatGpt.getModels();
        logger.debug("Navigated to SettingsPage.qml");
    }
}
