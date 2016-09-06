/*
 * File: generate_test_cases.cpp
 *
 * Copyright (C) 2016 Richard Eliáš <richard@ba30.eu>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 */

#include "generate_test_cases.hpp"
#include "rna_tree.hpp"
#include "utils.hpp"
#include "app.hpp"
#include "compact.hpp"
#include "document_writer.hpp"
#include "extractor.hpp"


#define parent(iter)    rna_tree::parent(iter)


static std::vector<std::unique_ptr<document_writer>> writers;
static rna_pair_label lbl_leaf("I");
static rna_pair_label lbl_pair = lbl_leaf + lbl_leaf;

using namespace std;

void f()
{
    LOGGER_PRIORITY_ON_FUNCTION(DEBUG);
    generate_test_cases test;
    test.run();
    exit(0);
}

static rna_tree get_rna(const std::string& filename)
{
    extractor& ex = extractor::get_extractor(filename + ".ps", "ps");
    string brackets = read_fasta_file(filename + ".fasta").brackets;

    rna_tree rna(brackets, ex.labels, ex.points);

    rna.print_tree();

    return rna;
}

#ifdef NODEF

void generate_test_cases::save_seq_fold_subtree(iterator it, string name)
{
    APP_DEBUG_FNAME;

#define OUTDIR_PART     (string("build/files/"))
    name = OUTDIR_PART + name;

    write_file(name + ".seq", rna_tree::get_labels(it));
    write_file(name + ".fold", rna_tree::get_brackets(it));
}

void generate_test_cases::generate()
{
    APP_DEBUG_FNAME;

    size_t n = 100;
    size_t i = 0;
    rna_tree rna = get_rna(INDIR + "frog");

    for (i = 0; i < 19; ++i)
    {
        iterator it = rna.begin();
        it = plusplus(it, n);

        while (rna_tree::is_leaf(it))
            ++it;

        string name = OUTDIR_PART + "rna_part." + to_string(i);
        save_seq_fold_subtree(it, name);
        save_to_psout(name + ".ps", it);

        n += 100;
    }
}

#endif

void generate_test_cases::save_seq_fold(rna_tree rna, std::string name)
{
    APP_DEBUG_FNAME;

    write_file(name + ".fasta", ">" + rna.name() + "\n" + rna.get_labels() + "\n" + rna.get_brackets());
}






std::vector<std::string> generate_test_cases::create_app_arguments(const std::string& file_in, const std::string& file_out)
{
    vector<string> vec = {
        "program_name",
        "-tt",
            file_in + ".ps",
            file_in + ".fasta",
        "-mt",
            file_out + ".fasta",
        //"-r",
            //"--strategies", file_out + ".matched.rted",
        //"-g",
            //"--ted-out", file_out + ".matched.ted",
            //"--mapping", file_out + ".matched.map",
        //"--ps",
            //"--mapping", file_out + ".matched.map",
            //file_out + ".matched.ps",
        "-a",
            "--colored",
            file_out ,
    };

    return vec;
}

void generate_test_cases::save_to_psout(const std::string& filename, iterator it)
{
    APP_DEBUG_FNAME;

    writers = document_writer::get_writers(true);

    for (const auto& writer : writers)
    {
        writer->init(filename, it);
        writer->print(writer->get_rna_subtree_formatted(it));
    }
}


void generate_test_cases::run()
{
    APP_DEBUG_FNAME;

    //run_hairpin();
    //run_interior();
    run_multibranch();
    run_multibranch_loop();
}


void generate_test_cases::insert_hairpin(
                rna_tree& rna,
                sibling_iterator ch,
                size_t n)
{
    int i = 4;
    while (--i != 0)
        ch = rna.insert(ch, lbl_leaf, 0);
    ch = rna.insert(ch, lbl_pair, 3);

    while (--n != 0)
        ch = rna.insert(ch, lbl_pair, 1);
}

void generate_test_cases::run_delete(size_t n, rna_tree& rna, sibling_iterator sib)
{
    while (n-- != 0)
        sib = rna.erase(sib);
}
void generate_test_cases::run_delete_leafs(size_t n, rna_tree& rna, sibling_iterator sib)
{
    while (n-- != 0)
    {
        assert(rna_tree::is_leaf(sib));
        sib = rna.erase(sib);
    }
}
void generate_test_cases::run_insert_leafs(size_t n, rna_tree& rna, sibling_iterator sib)
{
    while (n-- != 0)
        sib = rna.insert(sib, lbl_leaf, 0);
}
void generate_test_cases::run_insert(size_t n, rna_tree& rna, sibling_iterator sib)
{
    while (n-- != 0)
        sib = rna.insert(sib, lbl_pair, 1);
}


void generate_test_cases::run_app(std::string filetempl, std::string fileother)
{
    APP_DEBUG_FNAME;

    auto args = create_app_arguments(filetempl, fileother);

    app app;
    app.run(args);

    //rna_tree rna = get_rna(fileother);
    //save_to_psout(fileother + ".ps", ++rna.begin());
}

void generate_test_cases::run_hairpin()
{
    APP_DEBUG_FNAME;

#define FILEINDEX 0
    auto inserts = [this]()
    {
        string filein = FILEIN(FILEINDEX);
        string fileout = FILEOUT(FILEINDEX) + ".ins";

        rna_tree rna = get_rna(filein);
        sibling_iterator ch = plusplus(rna.begin(), 3);
        sibling_iterator l = plusplus(rna.begin(), 5);

        run_insert_leafs(5, rna, ch.begin());
        run_insert_leafs(1, rna, ch.end());
        run_insert_leafs(4, rna, l.begin());

        rna.print_tree();
        save_seq_fold(rna, fileout);
        run_app(filein, fileout);
    };
    auto deletes = [this]()
    {
        string filein = FILEIN(FILEINDEX);
        string fileout = FILEOUT(FILEINDEX) + ".del";

        rna_tree rna = get_rna(filein);
        sibling_iterator l = plusplus(rna.begin(), 5);

        run_delete_leafs(3, rna, l.begin());

        rna.print_tree();
        save_seq_fold(rna, fileout);
        run_app(filein, fileout);
    };

    inserts();
    deletes();

#undef FILEINDEX
}

void generate_test_cases::run_multibranch()
{
    APP_DEBUG_FNAME;

#define FILEINDEX 2

    auto deletes = [this]()
    {
        string filein = FILEIN(FILEINDEX);
        string fileout = FILEOUT(FILEINDEX) + ".del";

        rna_tree rna = get_rna(filein);
        sibling_iterator ch = plusplus(rna.begin(), 10);

        for (int i = 0; i < 14; ++i)
        {
            while (rna_tree::is_leaf(ch))
                ++ch;
            ch = rna.erase(ch);
        }

        rna.print_tree();
        save_seq_fold(rna, fileout);
        run_app(filein, fileout);
    };
    auto deletes_inserts = [this]()
    {
        string filein = FILEOUT(FILEINDEX) + ".del";
        string fileout = FILEOUT(FILEINDEX) + ".del-ins";

        rna_tree rna = get_rna(filein);
        sibling_iterator ch = plusplus(rna.begin(), 10);

        ch = rna.insert(ch, lbl_pair, 9);

        ch = plusplus(ch.begin(), 2);
        ch = rna.insert(ch, lbl_pair, 5);

        for (int i = 0; i < 7; ++i)
            rna.insert(ch, lbl_pair, 1);

        ch = plusplus(ch.begin(), 1);
        ch = rna.insert(ch, lbl_pair, 4);
        for (int i = 0; i < 4; ++i)
            rna.insert(ch, lbl_pair, 1);

        rna.print_tree();
        save_seq_fold(rna, fileout);
        run_app(filein, fileout);
    };
    auto inserts = [this]()
    {
        string filein = FILEIN(FILEINDEX);
        string fileout = FILEOUT(FILEINDEX) + ".ins";

        rna_tree rna = get_rna(filein);
        sibling_iterator ch = plusplus(rna.begin(), 6);

        ch = plusplus(ch.begin(), 4);

        run_insert_leafs(5, rna, ch);

        rna.print_tree();
        save_seq_fold(rna, fileout);
        run_app(filein, fileout);
    };

    deletes();
    deletes_inserts();
    inserts();
}

void generate_test_cases::run_multibranch_loop()
{
    APP_DEBUG_FNAME;

#define FILEINDEX 2

    auto deletes = [this]()
    {
        string filein = FILEIN(FILEINDEX);
        string fileout = FILEOUT(FILEINDEX) + ".del2";

        rna_tree rna = get_rna(filein);
        sibling_iterator ch = plusplus(rna.begin(), 8);

        for (int i = 0; i < 16; ++i)
        {
            while (rna_tree::is_leaf(ch))
                ++ch;
            ch = rna.erase(ch);
        }

        while (!ch->paired())
            ++ch;

        for (int i = 0; i < 2; ++i)
            ch = rna.erase(ch);

        rna.print_tree();
        save_seq_fold(rna, fileout);
        run_app(filein, fileout);
    };
    auto deletes_inserts = [this]()
    {
        string filein = FILEOUT(FILEINDEX) + ".del2";
        string fileout = FILEOUT(FILEINDEX) + ".del-ins2";

        rna_tree rna = get_rna(filein);
        sibling_iterator ch = plusplus(rna.begin(), 8);

        ch = rna.insert(ch, lbl_pair, 9);
        for (int i = 0; i < 2; ++i)
            ch = rna.insert(ch, lbl_pair, 1);

        ch = plusplus(rna_tree::iterator(ch.begin()), 2);
        ch = plusplus(ch, 2);
        ch = rna.insert(ch, lbl_pair, 5);

        for (int i = 0; i < 7; ++i)
            rna.insert(ch, lbl_pair, 1);

        ch = plusplus(ch.begin(), 1);
        ch = rna.insert(ch, lbl_pair, 4);
        for (int i = 0; i < 4; ++i)
            rna.insert(ch, lbl_pair, 1);

        ch = plusplus(rna_tree::iterator(ch), 10);

        ch = rna.insert(ch, lbl_pair, 4);
        ch = rna.insert(ch, lbl_pair, 1);

        //rna.print_tree();
        save_seq_fold(rna, fileout);
        run_app(filein, fileout);
    };

    deletes();
    deletes_inserts();
}




