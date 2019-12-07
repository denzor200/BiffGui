/***************************************
** Демонстрационная версия converter.exe
****************************************/

#include <QApplication>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <iostream>

static const QStringList s_NotSupportedMessage = {
    "Не поддерживается в демострационной версии программы.",
    "Приобретите полную версию (Справка->О программе->Контакты для связи)"
};

static const QString s_DefaultSettingsXml =
        "<?xml version=\"1.0\" encoding=\"utf-8\"?>             	"
        "<Root>                                                 	"
        "    <Additional>                                       	"
        "        <EnableSoundTheme>false</EnableSoundTheme>      	"
        "    </Additional>                                       	"
        "    <AssParsing>                                        	"
        "        <NeededColumns>                                 	"
        "            <NameColumn>Name</NameColumn>					"
        "            <TextColumn>Text</TextColumn>					"
        "            <TimeEndColumn>End</TimeEndColumn>				"
        "            <TimeStartColumn>Start</TimeStartColumn>		"
        "        </NeededColumns>									"
        "    </AssParsing>											"
        "    <DocumentStyle>										"
        "        <DisableLinesCounter>false</DisableLinesCounter>	"
        "        <DisableTags>false</DisableTags>					"
        "        <IndividualSelectedFont>							"
        "            <Bold>true</Bold>								"
        "            <Color>#000000</Color>							"
        "            <ColorBack>#ffff00</ColorBack>					"
        "            <Italic>false</Italic>							"
        "            <Name>Arial</Name>								"
        "            <Size>12.0</Size>								"
        "            <Underline>false</Underline>					"
        "        </IndividualSelectedFont>							"
        "        <MainFont>											"
        "            <Bold>false</Bold>								"
        "            <Color>#000000</Color>							"
        "            <ColorBack>#FFFFFF</ColorBack>					"
        "            <Italic>false</Italic>							"
        "            <Name>Arial</Name>								"
        "            <Size>12.0</Size>								"
        "            <Underline>false</Underline>					"
        "        </MainFont>										"
        "        <PageNumberFont>									"
        "            <Bold>false</Bold>								"
        "            <Color>#000000</Color>							"
        "            <ColorBack>#FFFFFF</ColorBack>					"
        "            <Italic>false</Italic>							"
        "            <Name>Calibri</Name>							"
        "            <Size>11.0</Size>								"
        "            <Underline>false</Underline>					"
        "        </PageNumberFont>									"
        "        <ParagraphInterval>12</ParagraphInterval>			"
        "    </DocumentStyle>										"
        "    <SrtParsing>											"
        "        <ComplexPhrases>									"
        "            <TimeDistributing>								"
        "                <Strategy>Fair</Strategy>					"
        "            </TimeDistributing>							"
        "        </ComplexPhrases>									"
        "    </SrtParsing>											"
        "    <Timing>												"
        "        <IntervalsDistributing>							"
        "            <Big>5000</Big>								"
        "            <Normal>2000</Normal>							"
        "            <VeryBig>8000</VeryBig>						"
        "            <VerySmall>1000</VerySmall>					"
        "            <VeryVeryBig>60000</VeryVeryBig>				"
        "        </IntervalsDistributing>							"
        "        <MilisecondRoundValue>700</MilisecondRoundValue>	"
        "        <PackingMethod>enable_intervals</PackingMethod>	"
        "    </Timing>												"
        "</Root>													";

static int not_supported ()
{
    QMessageBox::critical(nullptr, "Ошибка", s_NotSupportedMessage.join("\n"));
    return 1;
}

static int invalid_parameter()
{
    QMessageBox::critical(NULL, "Ошибка", "Параметры коммандной строки заданы неверно");
    return 1;
}

static int reset_settings() {
    QFile fileOut("settings.xml");
    if (fileOut.open(QIODevice::WriteOnly))
    {
        QTextStream streamFileOut(&fileOut);
        streamFileOut.setCodec("UTF-8");
        streamFileOut.setGenerateByteOrderMark(true);
        streamFileOut << s_DefaultSettingsXml;
        streamFileOut.flush();
        fileOut.close();
        return 0;
    }
    return 1;
}

static int make_docx() {
    for (const QString& value : s_NotSupportedMessage)
        std::cout <<"\"info\" \"" << value.toUtf8().data() << "\"" << std::endl;
    return 1;
}

static int show_person_list(const char* fileName) {
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    in.setCodec("UTF-8"); // change the file codec to UTF-8.

    std::cout <<"\"input_file_crc\" \"ffffffff:0\"" << std::endl;

    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line!="") {
            std::cout <<"\"person\" \"" << line.toUtf8().data() << "\"" << std::endl;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (argc < 2)
        return invalid_parameter();

    if (strcmp(argv[1], "reset_settings") == 0)
    {
        return reset_settings();
    }
    else if (strcmp(argv[1], "verify_cert") == 0)
    {
        return 1;
    }
    else if (strcmp(argv[1], "make_docx") == 0)
    {
        return make_docx();
    }
    else if (strcmp(argv[1], "make_subbtitle") == 0)
    {
        return not_supported();
    }
    else if (strcmp(argv[1], "show_person_list") == 0)
    {
        if (argc < 3)
            return invalid_parameter();
        return show_person_list(argv[2]);
    }

    return 0;
}
