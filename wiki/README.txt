3/24/15 - rohrlich

All of the wiki pages needed to support the Publish to Wiki feature of emergent are
exported as a single XML file.

When you export a revised set of pages remember to rename the file to PubToWikiPageExport.xml
so that it becomes a revision and not a new file when you check it in.

To export open the page Special:SpecialPages on the development wiki and paste the list of
pages that are in the file PubToWikiPageList.txt into the text box. This will download the
XML file you need.

If other properties or template pages, etc. become necessary for the Publish to Wiki
feature the filenames should be added to PubToWikiPageList.txt and that should be
checked in.

To import these pages into another wiki just go to that wiki's page Special:Import and
browse to the XML file.